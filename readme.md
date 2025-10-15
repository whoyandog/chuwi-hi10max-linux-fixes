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

## Make tablet-mode work with udev

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
