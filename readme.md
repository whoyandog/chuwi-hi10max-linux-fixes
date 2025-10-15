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

## Make tablet-mode work

1. Copy files

```bash
sudo cp udev/90-chuwi-tablet-mode-switch.rules /etc/udev/rules.d/
```
