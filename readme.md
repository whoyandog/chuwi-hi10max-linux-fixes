# Chuwi Hi10Max Linux fixes

---

## Accel matrix fix with udev

1. Copy udev rule:

```bash
sudo cp udev/90-chuwi-accel-matrix.rules /etc/udev/rules.d/
```

2. Reload udev rules

```bash
sudo udevadm control --reload
sudo udevadm trigger -s iio
```

3. Check that rule: 

```bash 
sudo systemctl restart iio-sensor-proxy
monitor-sensor
```
---

## Make tablet mode work

### With udev rules:

It is enough to set it once for it to always work. 

1. Copy files

```bash
sudo cp udev/90-chuwi-tablet-mode-switch.rules /etc/udev/rules.d/
sudo cp services/chuwi-tablet-mode-switch.service /etc/systemd/system/
sudo cp scripts/chuwi-tablet-mode-switch.py /usr/local/bin/
sudo cp services/chuwi-tablet-mode-boot.service /etc/systemd/system/
sudo cp scripts/chuwi-check-tablet-dock.sh /usr/local/bin/
```

2. Make sure that you installed evdev

Fedora: 
```bash
sudo dnf install python3-devel python3-pip gcc
sudo pip install evdev
```

3. Make scripts executalbe

```bash
sudo chmod +x /usr/local/bin/chuwi-tablet-mode-switch.py
sudo chmod +x /usr/local/bin/chuwi-check-tablet-dock.sh 
```

4. Reload udev and services

```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo systemctl daemon-reload
```

### With kernel driver: 

Works native, but requires recompilation after updating Linux kernel.

1. Compile driver for your kernel version: 

*Before compile you need to be sure that you have any packages for compilation.*

Fedora:
```bash
sudo dnf install @development-tools kernel-devel kernel-headers elfutils-libelf-devel
```

If you have the packages required for compilation, you can compile the driver: 
```bash
cd driver
make
```

2. Create necessary folders and install driver:

```bash
sudo mkdir -p /lib/modules/$(uname -r)/extra
sudo cp chuwi_switch_tablet_mode_driver.ko /lib/modules/$(uname -r)/extra/
sudo depmod -a
sudo modprobe chuwi_switch_tablet_mode_driver
```

3. Clean up artifact files:
```bash
make clean
cd ..
```

4. Make our driver autostart after boot:
Create .conf file:
```bash
sudo nano /etc/modules-load.d/chuwi_modules.conf
```
Insert into this file module name (without .ko):
```bash
chuwi_switch_tablet_mode_driver
```
To save in nano use ctrl+o, Enter, ctrl+x.

If Secure Boot is enabled, you need to sign the module or disable Secure Boot.

## License

Chuwi kernel driver is licensed under GPL-2.0-only. See `LICENSE` for details.