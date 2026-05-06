import { $ } from "bun";

const desktopPackages: Record<string, string[]> = {
  kde:   ["kde5", "kde5-baseapps", "sddm"],
  xfce:  ["xfce4", "lightdm", "lightdm-gtk3-greeter"],
  gnome: ["gnome", "gdm"],
};

export const install = {
  async start(payload: {
    device: string;
    username: string;
    password: string;
    desktop: string;
  }): Promise<void> {
    const { device, username, password, desktop } = payload;
    const hostname = "hollow";
    const pkgs = desktopPackages[desktop] ?? desktopPackages["kde"];

    // Mount partitions
    await $`mount ${device}2 /mnt`.quiet();
    await $`mkdir -p /mnt/boot/efi`.quiet();
    await $`mount ${device}1 /mnt/boot/efi`.quiet();

    // Install base system
    await $`xbps-install -S -R https://repo-default.voidlinux.org/current -r /mnt base-system`.quiet();

    // Install desktop
    await $`xbps-install -S -R https://repo-default.voidlinux.org/current -r /mnt ${pkgs.join(" ")}`.quiet();

    // Set hostname to hollow
    await $`echo "${hostname}" > /mnt/etc/hostname`.quiet();

    // Enable display manager service
    const dm = desktop === "kde" ? "sddm" : desktop === "xfce" ? "lightdm" : "gdm";
    await $`ln -s /etc/sv/${dm} /mnt/etc/runit/runsvdir/default/`.quiet();

    // Create user
    await $`chroot /mnt useradd -m -G wheel,audio,video,plugdev ${username}`.quiet();
    await $`echo "${username}:${password}" | chroot /mnt chpasswd`.quiet();

    // Install bootloader
    await $`chroot /mnt grub-install --target=x86_64-efi --efi-directory=/boot/efi`.quiet();
    await $`chroot /mnt grub-mkconfig -o /boot/grub/grub.cfg`.quiet();

    // Unmount
    await $`umount -R /mnt`.quiet();
  },
};
