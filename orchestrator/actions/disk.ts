import { $ } from "bun";

export const disk = {
  async list(): Promise<{ device: string; size: string; type: string }[]> {
    const result = await $`lsblk -J -o NAME,SIZE,TYPE`.quiet().text();
    const json = JSON.parse(result);
    return json.blockdevices
      .filter((d: any) => d.type === "disk")
      .map((d: any) => ({
        device: `/dev/${d.name}`,
        size: d.size,
        type: d.type,
      }));
  },

  async partition(payload: { device: string }): Promise<void> {
    const { device } = payload;

    // Partition table
    await $`parted -s ${device} mklabel gpt`.quiet();
    await $`parted -s ${device} mkpart ESP fat32 1MiB 513MiB`.quiet();
    await $`parted -s ${device} set 1 esp on`.quiet();
    await $`parted -s ${device} mkpart primary 513MiB 100%`.quiet();

    // Format EFI
    await $`mkfs.fat -F32 ${device}1`.quiet();

    // Format root as Btrfs
    await $`mkfs.btrfs -f ${device}2`.quiet();

    // Mount and create subvolumes
    await $`mount ${device}2 /mnt`.quiet();
    await $`btrfs subvolume create /mnt/@`.quiet();
    await $`btrfs subvolume create /mnt/@home`.quiet();
    await $`btrfs subvolume create /mnt/@snapshots`.quiet();
    await $`umount /mnt`.quiet();

    // Remount with subvolumes
    await $`mount -o subvol=@,compress=zstd /mnt`.quiet();
    await $`mkdir -p /mnt/home`.quiet();
    await $`mkdir -p /mnt/.snapshots`.quiet();
    await $`mkdir -p /mnt/boot/efi`.quiet();
    await $`mount -o subvol=@home,compress=zstd ${device}2 /mnt/home`.quiet();
    await $`mount -o subvol=@snapshots,compress=zstd ${device}2 /mnt/.snapshots`.quiet();
    await $`mount ${device}1 /mnt/boot/efi`.quiet();
  },
};
