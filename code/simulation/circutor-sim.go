package main

import (
	"fmt"
	"math/rand"
	"os"
	"path/filepath"
	"time"

	"github.com/tbrandon/mbserver"
)

func main() {
	// 1. Initialize the Modbus Server
	serv := mbserver.NewServer()
	err := serv.ListenTCP("127.0.0.1:5020")
	if err != nil {
		fmt.Printf("Failed to start server: %v\n", err)
		return
	}

	// 2. Background Task: Update Modbus Registers & Virtual Pins
	go func() {
		parentDir := "../virtual_pins"
		
		// Ensure the directory exists (Uses 'os')
		_ = os.MkdirAll(parentDir, 0755)
		fmt.Printf("Simulator active. Writing pins to: %s\n", parentDir)

		for {
			// --- Wiggle Modbus Values (Uses 'math/rand') ---
			// Voltage: 230V +/- 5V (2250 to 2350)
			voltage := 2250 + rand.Intn(100)
			serv.HoldingRegisters[0] = uint16(0)
			serv.HoldingRegisters[1] = uint16(voltage)

			// Temperature: 34.0C +/- 2C (320 to 360)
			temp := 320 + rand.Intn(40)
			serv.HoldingRegisters[80] = uint16(0)
			serv.HoldingRegisters[81] = uint16(temp)

			// --- Randomize Virtual Pins (Uses 'os' and 'path/filepath') ---
			entries, _ := os.ReadDir(parentDir)
			for _, entry := range entries {
				if entry.IsDir() {
					valPath := filepath.Join(parentDir, entry.Name(), "value")
					newVal := "0"
					if rand.Float32() > 0.5 {
						newVal = "1"
					}
					_ = os.WriteFile(valPath, []byte(newVal), 0644)
				}
			}

			time.Sleep(2 * time.Second)
		}
	}()

	fmt.Println("Circutor Simulator listening on 127.0.0.1:5020")
	fmt.Println("\nRegister  0: Voltage")
	fmt.Println("Register 80: Temp")
	fmt.Println("\nPress Ctrl+C to stop")
	
	select {} // Block forever
}
