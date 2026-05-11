# AeroCam Firmware Bring-Up Guide
MIT License  
© 2026 Don Michael Feeney Jr

---

# 1. Hardware Wiring

## ICM-20948 (SPI Mode)
| IMU Pin | STM32 Pin | Notes |
|--------|-----------|-------|
| VCC    | 3.3V      | Do NOT use 5V |
| GND    | GND       | Common ground required |
| SCL/SCLK | SPI1_SCK | 10k pull-down recommended |
| SDA/SDI | SPI1_MOSI | Push-pull |
| ADO/SDO | SPI1_MISO | Floating OK |
| CS     | GPIO (PA4 recommended) | Active low |

**Recommended:**  
- 100nF decoupling capacitor near IMU  
- Keep SPI traces short  
- Shielded cable if >10cm  

---

# 2. Motor Driver PWM Wiring

| Axis | STM32 Pin | Timer Channel |
|------|-----------|----------------|
| Roll | e.g., PA8 | TIM1_CH1 |
| Pitch | e.g., PA9 | TIM1_CH2 |
| Yaw | e.g., PA10 | TIM1_CH3 |

PWM frequency: **200–400 Hz**  
Pulse range: **1000–2000 µs**

---

# 3. Clocking Requirements

## STM32F4
- External crystal: **8 MHz**
- PLL → **168 MHz SYSCLK**
- APB1: 42 MHz  
- APB2: 84 MHz  

## STM32F7
- External crystal: **8 MHz**
- PLL → **216 MHz SYSCLK**
- Flash latency: 7 WS  

## STM32H7
- External crystal: **25 MHz**
- PLL → **400 MHz SYSCLK**
- Supply: LDO or SMPS  
- Voltage scaling: VOS1  

---

# 4. Control Loop Timing

## 1 kHz Control Loop
- TIM2 configured for **1 ms period**
- ISR sets `g_tick = true`
- Main loop processes stabilizer update

## IMU ODR
- ICM-20948 configured for **4 kHz**
- SPI burst read every 1 ms
- DMA recommended for low jitter

## Latency Expectations
- SPI DMA transfer: ~20–40 µs
- EKF update: < 50 µs on F4, < 20 µs on H7
- Total loop budget: < 200 µs

---

# 5. Firmware Bring-Up Steps

### Step 1 — Flash Firmware
Using ST-Link:

st-flash write AeroCam.bin 0x08000000


### Step 2 — Verify SWD
Pins required:
- SWDIO  
- SWCLK  
- NRST  
- GND  

### Step 3 — Check IMU Communication
- Confirm CS toggles  
- Confirm SPI clock present  
- Read WHO_AM_I (0xEA expected)  

### Step 4 — Check PWM Outputs
- Use oscilloscope or logic analyzer  
- Expect 1–2 ms pulses at 200–400 Hz  

### Step 5 — First Motion Test
- Run stabilizer in FILM_MODE  
- Verify smooth damping  
- Switch to AERO_MODE for aggressive response  

---

# 6. Safety Notes

- **No dynamic memory** anywhere in the firmware  
- All buffers must be static or stack-allocated  
- Enable watchdog timer for field reliability  
- Ensure IMU orientation is correct before flight 
