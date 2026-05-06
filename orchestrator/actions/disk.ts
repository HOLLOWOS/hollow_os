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
    // EFI partition 512MB, rest is root
    await $`parted -s ${device} mklabel gpt`.quiet();
    await $`parted -s ${device} mkpart ESP fat32 1MiB 513MiB`.quiet();
    await $`parted -s ${device} set 1 esp on`.quiet();
    await $`parted -s ${device} mkpart primary ext4 513MiB 100%`.quiet();
    // Format
    await $`mkfs.fat -F32 ${device}1`.quiet();
    await $`mkfs.ext4 ${device}2`.quiet();
  },
};
