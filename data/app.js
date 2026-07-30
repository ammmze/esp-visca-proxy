"use strict";

const $ = (s) => document.querySelector(s);
const $$ = (s) => document.querySelectorAll(s);

async function api(path, body) {
  const opts = body
    ? { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) }
    : {};
  const r = await fetch(path, opts);
  return r.json().catch(() => ({}));
}

const ptz = (body) => api("/api/ptz", body);

// ---- tabs ----
$$("#tabs button").forEach((b) => {
  b.onclick = () => {
    $$("#tabs button").forEach((x) => x.classList.remove("active"));
    $$(".tab").forEach((x) => x.classList.remove("active"));
    b.classList.add("active");
    $("#" + b.dataset.tab).classList.add("active");
    if (b.dataset.tab === "status") loadStatus();
  };
});

// ---- speed sliders ----
const panSpeed = $("#panSpeed"), tiltSpeed = $("#tiltSpeed"), zoomSpeed = $("#zoomSpeed");
panSpeed.oninput = () => ($("#panSpeedV").textContent = panSpeed.value);
tiltSpeed.oninput = () => ($("#tiltSpeedV").textContent = tiltSpeed.value);

// ---- pan/tilt pad: press to move, release to stop ----
$$(".pad button[data-pan]").forEach((btn) => {
  const start = (e) => {
    e.preventDefault();
    ptz({
      action: "move",
      pan: btn.dataset.pan,
      tilt: btn.dataset.tilt,
      panSpeed: +panSpeed.value,
      tiltSpeed: +tiltSpeed.value,
    });
  };
  const stop = () => ptz({ action: "stop" });
  btn.addEventListener("mousedown", start);
  btn.addEventListener("touchstart", start, { passive: false });
  btn.addEventListener("mouseup", stop);
  btn.addEventListener("mouseleave", stop);
  btn.addEventListener("touchend", stop);
});
$("#home").onclick = () => ptz({ action: "home" });

// ---- zoom: press/release ----
$$("[data-zoom]").forEach((btn) => {
  const start = (e) => { e.preventDefault(); ptz({ action: "zoom", dir: btn.dataset.zoom, speed: +zoomSpeed.value }); };
  const stop = () => ptz({ action: "zoom", dir: "stop" });
  btn.addEventListener("mousedown", start);
  btn.addEventListener("touchstart", start, { passive: false });
  btn.addEventListener("mouseup", stop);
  btn.addEventListener("mouseleave", stop);
  btn.addEventListener("touchend", stop);
});

// ---- focus ----
$$("[data-focus]").forEach((btn) => {
  const start = (e) => { e.preventDefault(); ptz({ action: "focus", dir: btn.dataset.focus }); };
  const stop = () => ptz({ action: "focus", dir: "stop" });
  btn.addEventListener("mousedown", start);
  btn.addEventListener("touchstart", start, { passive: false });
  btn.addEventListener("mouseup", stop);
  btn.addEventListener("mouseleave", stop);
  btn.addEventListener("touchend", stop);
});
$$("[data-focusauto]").forEach((btn) => {
  btn.onclick = () => ptz({ action: "focus", auto: btn.dataset.focusauto === "1" });
});

// ---- power ----
$$("[data-power]").forEach((btn) => {
  btn.onclick = () => ptz({ action: "power", on: btn.dataset.power === "1" });
});

// ---- presets ----
const grid = $("#presetgrid");
for (let i = 1; i <= 12; i++) {
  const b = document.createElement("button");
  b.textContent = i;
  b.onclick = () => {
    const op = document.querySelector('input[name="pmode"]:checked').value;
    api("/api/preset", { op, id: i });
  };
  grid.appendChild(b);
}

// ---- raw console ----
$("#rawsend").onclick = async () => {
  const hex = $("#rawhex").value.trim();
  if (!hex) return;
  const res = await api("/api/visca/raw", { hex });
  $("#rawout").textContent = "» " + hex + "\n« " + (res.reply || "(no reply)");
};

// ---- settings ----
async function loadConfig() {
  const c = await api("/api/config");
  const f = $("#cfgform");
  for (const [k, v] of Object.entries(c)) {
    const el = f.elements[k];
    if (!el) continue;
    if (el.type === "checkbox") el.checked = !!v;
    else el.value = v;
  }
}
$("#cfgform").onsubmit = async (e) => {
  e.preventDefault();
  const f = e.target;
  const body = {};
  for (const el of f.elements) {
    if (!el.name) continue;
    if (el.type === "checkbox") body[el.name] = el.checked;
    else if (el.type === "password") { if (el.value) body[el.name] = el.value; }
    else if (el.type === "number") body[el.name] = +el.value;
    else body[el.name] = el.value;
  }
  const res = await api("/api/config", body);
  alert(res.reboot ? "Saved. Rebooting…" : "Saved.");
};
$("#reboot").onclick = () => { if (confirm("Reboot device?")) api("/api/reboot", {}); };
$("#factory").onclick = () => { if (confirm("Erase all settings?")) api("/api/factory-reset", {}); };

// ---- status ----
async function loadStatus() {
  const s = await api("/api/status");
  $("#statusout").textContent = JSON.stringify(s, null, 2);
  const badge = $("#linkbadge");
  badge.textContent = s.captive ? "setup mode" : s.link + " · " + s.ip;
  badge.classList.toggle("online", s.link && s.link !== "offline");
  if (s.source) $("#srclink").href = s.source;
  if (s.fw) $("#fwver").textContent = s.name + " v" + s.fw;
}
$("#refresh").onclick = loadStatus;

// ---- init ----
loadConfig();
loadStatus();
