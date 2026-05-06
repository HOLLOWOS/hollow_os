import { $ } from "bun";

export const wifi = {
  async scan(): Promise<{ ssid: string; signal: string }[]> {
    await $`iw dev wlp107s0 scan`.quiet();
    await Bun.sleep(2000);
    const result = await $`iw dev wlp107s0 scan`.quiet().text();
    return parseNetworks(result);
  },

  async connect(payload: { ssid: string; password?: string }): Promise<void> {
    const { ssid, password } = payload;
    if (password) {
      await $`printf 'network={\nssid="${ssid}"\npsk="${password}"\n}' > /tmp/hollow-wpa.conf`.quiet();
      await $`wpa_supplicant -B -i wlp107s0 -c /tmp/hollow-wpa.conf`.quiet();
    } else {
      await $`wpa_supplicant -B -i wlp107s0`.quiet();
    }
    await Bun.sleep(3000);
    await $`dhcpcd wlp107s0`.quiet();
  },
};

function parseNetworks(raw: string) {
  const networks: { ssid: string; signal: string }[] = [];
  const lines = raw.split("\n");
  for (const line of lines) {
    const ssidMatch = line.match(/SSID: (.+)/);
    const signalMatch = line.match(/signal: (.+) dBm/);
    if (ssidMatch) {
      networks.push({
        ssid: ssidMatch[1].trim(),
        signal: signalMatch ? signalMatch[1].trim() + " dBm" : "?"
      });
    }
  }
  return networks;
}
