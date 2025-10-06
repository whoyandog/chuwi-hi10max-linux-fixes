# Chuwi Hi10Max Linux fixes

## Accel matrix fix with udev

1. Copy udev rule:

```bash
sudo cp udev/90-accel-matrix.rules /etc/udev/rules.d/
sudo udevadm control --reload
sudo udevadm trigger -s iio
```

2. Check that rule: 

```bash 
sudo systemctl restart iio-sensor-proxy
monitor-system
```
