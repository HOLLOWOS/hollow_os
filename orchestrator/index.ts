import { wifi } from "./actions/wifi";
import { disk } from "./actions/disk";
import { install } from "./actions/install";
import { update } from "./actions/update";

type Message = {
  id: string;
  action: string;
  payload?: any;
};

async function dispatch(msg: Message): Promise<any> {
  switch (msg.action) {
    case "wifi.scan":        return wifi.scan();
    case "wifi.connect":     return wifi.connect(msg.payload);
    case "disk.list":        return disk.list();
    case "disk.partition":   return disk.partition(msg.payload);
    case "install.start":    return install.start(msg.payload);
    case "update.run":       return update.run();
    case "update.list":      return update.list();
    case "update.snapshot":  return update.snapshot(msg.payload.name);
    case "update.rollback":  return update.rollback(msg.payload.name);
    default:
      throw new Error(`hollow-orc: unknown action: ${msg.action}`);
  }
}

function respond(obj: object) {
  process.stdout.write(JSON.stringify(obj) + "\n");
}

async function main() {
  const decoder = new TextDecoder();
  for await (const chunk of Bun.stdin.stream()) {
    const lines = decoder.decode(chunk).trim().split("\n");
    for (const line of lines) {
      if (!line.trim()) continue;
      let msg: Message;
      try {
        msg = JSON.parse(line);
      } catch {
        respond({ id: "?", ok: false, error: "hollow-orc: invalid JSON" });
        continue;
      }
      try {
        const data = await dispatch(msg);
        respond({ id: msg.id, ok: true, data });
      } catch (e: any) {
        respond({ id: msg.id, ok: false, error: e.message });
      }
    }
  }
}

main();
