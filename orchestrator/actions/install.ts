import { $ } from "bun";

export const install = {
  async start(payload: { device: string; username: string; password: string; hostname: string }): Promise<void> {
    const { device, username, password } = payload;
    const hostname = "hollow";

    // Mount partitions
    await $`mount ${device}2 /mnt`.quiet();
    await $`mkdir -p /mnt/boot/efi`.quiet();
    await $`mount ${device}1 /mnt/boot/efi`.quiet();

    // Install base system
    await $`xbps-install -S -R https://repo-default.voidlinux.org/current -r /mnt base-system`.quiet();

    // Set hostname to hollow
    await $`echo "${hostname}" > /mnt/etc/hostname`.quiet();

    // Set root password
    await $`chroot /mnt passwd root`.quiet();

    // Create user
    await $`chroot /mnt useradd -m -G wheel,audio,video ${username}`.quiet();
    await $`echo "${username}:${password}" | chroot /mnt chpasswd`.quiet();

    // Install bootloader
    await $`chroot /mnt grub-install --target=x86_64-efi --efi-directory=/boot/efi`.quiet();
    await $`chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg`.quiet();

    // Unmount
    await $`umount -R /mnt`.quiet();
  },
};
