import { $ } from "bun";

export const update = {
  async snapshot(name: string): Promise<void> {
    const date = new Date().toISOString().slice(0, 10);
    const snapName = `${date}-${name}`;
    await $`btrfs subvolume snapshot / /.snapshots/${snapName}`.quiet();
  },

  async list(): Promise<{ name: string; date: string }[]> {
    const result = await $`btrfs subvolume list /.snapshots`.quiet().text();
    return result
      .split("\n")
      .filter(Boolean)
      .map((line) => {
        const parts = line.split(" ");
        const name = parts[parts.length - 1].replace("/.snapshots/", "");
        const date = name.split("-").slice(0, 3).join("-");
        return { name, date };
      });
  },

  async rollback(name: string): Promise<void> {
    // Swap current @ with snapshot
    await $`btrfs subvolume snapshot /.snapshots/${name} /@_rollback`.quiet();
    await $`btrfs subvolume delete /@`.quiet();
    await $`mv /@_rollback /@`.quiet();
  },

  async run(): Promise<void> {
    // Take pre-update snapshot
    await update.snapshot("pre-update");
    // Run xbps update
    await $`xbps-install -Su`.quiet();
    // Take post-update snapshot
    await update.snapshot("post-update");
  },
};
