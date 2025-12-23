package waytext

/*
#cgo pkg-config: wayland-client
#include <stdlib.h>
#include "text_input.h"
*/
import "C"
import (
	"sync"
	"unsafe"
)

var (
	initOnce sync.Once
	initErr  error
)

// Init initializes the Wayland text-input backend.
// Must be called once per process.
func Init() error {
	initOnce.Do(func() {
		C.wl_init()
		// wl_init has no error channel; failure means no effect at runtime
	})
	return initErr
}

// Type commits UTF-8 text into the currently focused Wayland text input.
// Works for Arabic, CJK, emoji.
func Type(text string) error {
	if text == "" {
		return nil
	}

	cs := C.CString(text)
	defer C.free(unsafe.Pointer(cs))

	C.wl_commit_text(cs)
	return nil
}

// Close releases Wayland resources.
// Optional. Safe to call once at shutdown.
func Close() {
	C.wl_cleanup()
}
