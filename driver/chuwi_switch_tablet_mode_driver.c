// SPDX-License-Identifier: GPL-2.0-only
/*
 * chuwi_switch_tablet_mode_driver.c - Linux kernel driver for switchable tablet mod with dock-keyboard
 *
 * Copyright (C) 2025 Dmitry <https://github.com/whoyandog>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/usb.h>
#include <linux/notifier.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/atomic.h>

#define DRIVER_NAME "CHUWI switch tablet mode"

static unsigned short vendor_id = 0x258a;
static unsigned short product_id = 0x0020;
module_param(vendor_id, ushort, 0444);
MODULE_PARM_DESC(vendor_id, "USB vendor id to watch (hex)");
module_param(product_id, ushort, 0444);
MODULE_PARM_DESC(product_id, "USB product id to watch (hex)");

static struct input_dev *switch_dev;
static atomic_t device_count = ATOMIC_INIT(0); 

static void switch_tablet_mode(bool tablet);

static inline bool usb_match_vidpid(const struct usb_device *udev)
{
    if (!udev)
        return false;

    if (le16_to_cpu(udev->descriptor.idVendor) == vendor_id &&
        le16_to_cpu(udev->descriptor.idProduct) == product_id)
        return true;

    return false;
}

static int find_cb(struct usb_device *udev, void *data)
{
    if (usb_match_vidpid(udev)) {
        atomic_inc(&device_count);
    }
    return 0; /* continue */
}

static int usb_nb_cb(struct notifier_block *nb, unsigned long action, void *data)
{
    struct usb_device *udev = NULL;
    bool should_report = false;
    int val = 0;

    switch (action) {
    case USB_DEVICE_ADD:
        udev = data;
        if (!usb_match_vidpid(udev))
            break;
        val = atomic_inc_return(&device_count);
        /* if this is the first attached device, we leave "not-tablet" state */
        if (val == 1) {
            should_report = true;
        }
        break;

    case USB_DEVICE_REMOVE:
        udev = data;
        if (!usb_match_vidpid(udev))
            break;
        val = atomic_dec_return(&device_count);
        if (val < 0) {
            atomic_set(&device_count, 0);
            val = 0;
        }
        if (val == 0) {
            should_report = true;
        }
        break;

    default:
        break;
    }

    if (should_report && switch_dev) {
        bool tablet = (atomic_read(&device_count) == 0);
        switch_tablet_mode(tablet);
    }

    return NOTIFY_OK;
}

static struct notifier_block usb_nb = {
    .notifier_call = usb_nb_cb,
};

static void switch_tablet_mode(bool tablet_mode)
{
    if (!switch_dev)
        return;

    input_report_switch(switch_dev, SW_TABLET_MODE, (int)tablet_mode);
    input_sync(switch_dev);
    pr_info("%s: reported SW_TABLET_MODE=%d\n", DRIVER_NAME, (int)tablet_mode);
}

static int __init switch_tablet_mode_init(void)
{
    int err;
    int initial_count;

    pr_info("%s: init (watching %04x:%04x)\n", DRIVER_NAME, vendor_id, product_id);

    /* allocate a virtual input device that will be always present */
    switch_dev = input_allocate_device();
    if (!switch_dev) {
        pr_err("%s: failed to allocate input device\n", DRIVER_NAME);
        return -ENOMEM;
    }

    switch_dev->name = "CHUWI switch tablet mode virtual device";
    switch_dev->phys = "dock/switch";
    __set_bit(EV_SW, switch_dev->evbit);
    __set_bit(SW_TABLET_MODE, switch_dev->swbit);

    err = input_register_device(switch_dev);
    if (err) {
        pr_err("%s: input_register_device failed: %d\n", DRIVER_NAME, err);
        input_free_device(switch_dev);
        switch_dev = NULL;
        return err;
    }

    /* count any already-present matching devices */
    atomic_set(&device_count, 0);
    usb_for_each_dev(NULL, find_cb);
    initial_count = atomic_read(&device_count);

    /* report initial state: tablet if no device present */
    switch_tablet_mode(initial_count == 0);

    /* register for USB add/remove notifications */
    usb_register_notify(&usb_nb);
    
    return 0;
}

static void __exit switch_tablet_mode_exit(void)
{
    pr_info("%s: exit\n", DRIVER_NAME);

    usb_unregister_notify(&usb_nb);

    if (switch_dev) {
        input_unregister_device(switch_dev);
        switch_dev = NULL;
    }
}

module_init(switch_tablet_mode_init);
module_exit(switch_tablet_mode_exit);

MODULE_AUTHOR("Dmitry <https://github.com/whoyandog>");
MODULE_DESCRIPTION("Persistent module that toggles SW_TABLET_MODE when a Chuwi dock keyboard is attached/removed");
MODULE_LICENSE("GPL");

