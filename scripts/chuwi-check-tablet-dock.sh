#!/bin/sh
VENDOR="258a"
PRODUCT="0020"

/usr/bin/udevadm settle --timeout=3 2>/dev/null || true

found=0
for d in /sys/bus/usb/devices/*; do
    [ -f "$d/idVendor" ] || continue
    [ -f "$d/idProduct" ] || continue
    v=$(cat "$d/idVendor" 2>/dev/null || echo)
    p=$(cat "$d/idProduct" 2>/dev/null || echo)
    if [ "$v" = "$VENDOR" ] && [ "$p" = "$PRODUCT" ]; then
        found=1
        break
    fi
done

if [ "$found" -eq 1 ]; then
    /usr/bin/systemctl --no-block stop chuwi-tablet-mode-switch.service || true
else
    /usr/bin/systemctl --no-block start chuwi-tablet-mode-switch.service || true
fi

exit 0

