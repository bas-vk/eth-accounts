package main

/*
#include <string.h>
#include <stdlib.h>
*/
import "C"

import (
	"github.com/ethereum/go-ethereum/accounts"
	"github.com/ethereum/go-ethereum/common"

	"errors"
	"encoding/hex"
	"sync"
	"unsafe"
)

const (
	maxErrorLength = 1024
)

var (
	mu       sync.Mutex
	count    int
	managers map[int]*accounts.Manager
	tmp      string

	ErrUnknownAccountManager = errors.New("no account manager found")
)

func init() {
	managers = make(map[int]*accounts.Manager)
}

//export NewAccountManager
func NewAccountManager(keydir *C.char) int {
	scryptN := accounts.StandardScryptN
	scryptP := accounts.StandardScryptP

	kd := C.GoString(keydir)

	newManager := accounts.NewManager(kd, scryptN, scryptP)
	mu.Lock()
	defer mu.Unlock()

	id := count
	managers[id] = newManager
	count++
	return id
}

//export DeleteAccountManager
func DeleteAccountManager(id int) {
	mu.Lock()
	delete(managers, id)
	mu.Unlock()
}

func managerNotFound(error *C.char) bool {
	errMsg := C.CString(ErrUnknownAccountManager.Error())
	defer C.free(unsafe.Pointer(errMsg))
	errMsgLen := C.size_t(len(ErrUnknownAccountManager.Error()))

	C.strncpy(error, errMsg, errMsgLen)
	return false
}

//export NewAccount
func NewAccount(id int, passphrase string, address *C.char, error *C.char) bool {
	manager, found := managers[id]
	if !found {
		return managerNotFound(error)
	}

	account, err := manager.NewAccount(passphrase)
	if err != nil {
		errMsg := C.CString(err.Error())
		defer C.free(unsafe.Pointer(errMsg))
		errMsgLen := C.size_t(len(err.Error()))
		if len(err.Error()) > maxErrorLength {
			errMsgLen = C.size_t(maxErrorLength)
		}

		C.strncpy(error, errMsg, errMsgLen)
		return false
	}

	addr := C.CString(account.Address.Hex())
	defer C.free(unsafe.Pointer(addr))
	addrLen := C.size_t(len(account.Address.Hex()))

	C.strncpy(address, addr, addrLen)

	return true
}

//export UnlockAccount
func UnlockAccount(id int, address, passphrase string, error *C.char) bool {
	manager, found := managers[id]
	if !found {
		return managerNotFound(error)
	}
	
	acc := accounts.Account{Address: common.HexToAddress(address)}
	err := manager.Unlock(acc, passphrase)
	if err != nil {
		errMsg := C.CString(err.Error())
		defer C.free(unsafe.Pointer(errMsg))
		errMsgLen := C.size_t(len(err.Error()))
		if len(err.Error()) > maxErrorLength {
			errMsgLen = C.size_t(maxErrorLength)
		}
		
		C.strncpy(error, errMsg, errMsgLen)
		return false
	}

	return true
}

//export LockAccount
func LockAccount(id int, address string) bool {
	manager, found := managers[id]
	if !found {
		return false
	}
	
	addr := common.HexToAddress(address)
	return manager.Lock(addr) == nil
}

//export Accounts
func Accounts(id int, accounts *string) int {
	manager, found := managers[id]
	if !found {
		return 0
	}
	
	accs := manager.Accounts()
	var addresses string
	for _, a := range accs {
		addresses += a.Address.Hex()
	}
	
	*accounts = addresses
		
	return len(accs)
}

//export Sign
func Sign(id int, account string, hash string, signature *string, error *C.char) bool {
	manager, found := managers[id]
	if !found {
		return managerNotFound(error)
	}
	
	addr := common.HexToAddress(account)
	if len(hash) == 66 {
		hash = hash[2:]
	}
	h, _ := hex.DecodeString(hash)
	
	sig, err := manager.Sign(addr, h)
	if err != nil {
		errMsg := C.CString(err.Error())
		defer C.free(unsafe.Pointer(errMsg))
		errMsgLen := C.size_t(len(err.Error()))
		if len(err.Error()) > maxErrorLength {
			errMsgLen = C.size_t(maxErrorLength)
		}
		
		C.strncpy(error, errMsg, errMsgLen)
		return false
	}
	
	*signature = common.ToHex(sig)	
	return true
}

func main() {
}
