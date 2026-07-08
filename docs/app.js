const canvas = document.getElementById("simCanvas");
const ctx = canvas.getContext("2d");

const gravityStatus = document.getElementById("gravityStatus");
const windStatus = document.getElementById("windStatus");
const tearStatus = document.getElementById("tearStatus");

let width = canvas.width;
let height = canvas.height;

let particles = [];
let constraints = [];

let gravityEnabled = true;
let windEnabled = false;
let tearingEnabled = true;

let mouse = {
  x: width / 2,
  y: height / 2,
  down: false,
  button: 0,
  grabbed: null
};

const SETTINGS = {
  gravity: 0.45,
  friction: 0.994,
  bounce: 0.22,
  particleRadius: 5,
  constraintIterations: 7,
  tearDistanceFactor: 2.15,
  cutRadius: 34,
  grabRadius: 55,
  windStrength: 0.22
};

class Particle {
  constructor(x, y, pinned = false) {
    this.x = x;
    this.y = y;
    this.oldX = x;
    this.oldY = y;
    this.pinned = pinned;
    this.radius = SETTINGS.particleRadius;
  }

  update() {
    if (this.pinned) return;

    const vx = (this.x - this.oldX) * SETTINGS.friction;
    const vy = (this.y - this.oldY) * SETTINGS.friction;

    this.oldX = this.x;
    this.oldY = this.y;

    this.x += vx;
    this.y += vy;

    if (gravityEnabled) {
      this.y += SETTINGS.gravity;
    }

    if (windEnabled) {
      this.x += Math.sin(performance.now() * 0.002 + this.y * 0.02) * SETTINGS.windStrength;
    }
  }

  constrainToBounds() {
    if (this.pinned) return;

    const vx = this.x - this.oldX;
    const vy = this.y - this.oldY;

    if (this.x > width - this.radius) {
      this.x = width - this.radius;
      this.oldX = this.x + vx * SETTINGS.bounce;
    }

    if (this.x < this.radius) {
      this.x = this.radius;
      this.oldX = this.x + vx * SETTINGS.bounce;
    }

    if (this.y > height - this.radius) {
      this.y = height - this.radius;
      this.oldY = this.y + vy * SETTINGS.bounce;
    }

    if (this.y < this.radius) {
      this.y = this.radius;
      this.oldY = this.y + vy * SETTINGS.bounce;
    }
  }

  draw() {
    ctx.beginPath();
    ctx.arc(this.x, this.y, this.pinned ? this.radius + 2 : this.radius, 0, Math.PI * 2);
    ctx.fillStyle = this.pinned ? "#f92672" : "#00b0ff";
    ctx.fill();
  }
}

class Constraint {
  constructor(a, b, length = null, strength = 1) {
    this.a = a;
    this.b = b;
    this.length = length ?? distance(a, b);
    this.strength = strength;
  }

  solve() {
    const dx = this.b.x - this.a.x;
    const dy = this.b.y - this.a.y;
    const dist = Math.sqrt(dx * dx + dy * dy);

    if (dist === 0) return true;

    if (tearingEnabled && dist > this.length * SETTINGS.tearDistanceFactor) {
      return false;
    }

    const diff = (this.length - dist) / dist;
    const offsetX = dx * diff * 0.5 * this.strength;
    const offsetY = dy * diff * 0.5 * this.strength;

    if (!this.a.pinned) {
      this.a.x -= offsetX;
      this.a.y -= offsetY;
    }

    if (!this.b.pinned) {
      this.b.x += offsetX;
      this.b.y += offsetY;
    }

    return true;
  }

  draw() {
    ctx.beginPath();
    ctx.moveTo(this.a.x, this.a.y);
    ctx.lineTo(this.b.x, this.b.y);

    const stretch = distance(this.a, this.b) / this.length;
    ctx.strokeStyle = stretch > 1.6 ? "#ffc900" : "rgba(245, 245, 247, 0.75)";
    ctx.lineWidth = 1.2;
    ctx.stroke();
  }
}

function distance(a, b) {
  const dx = b.x - a.x;
  const dy = b.y - a.y;
  return Math.sqrt(dx * dx + dy * dy);
}

function addParticle(x, y, pinned = false) {
  const p = new Particle(x, y, pinned);
  particles.push(p);
  return p;
}

function addConstraint(a, b, length = null, strength = 1) {
  const c = new Constraint(a, b, length, strength);
  constraints.push(c);
  return c;
}

function clearWorld() {
  particles = [];
  constraints = [];
  mouse.grabbed = null;
}

function spawnRope(x, y) {
  let previous = null;
  const count = 16;
  const gap = 24;

  for (let i = 0; i < count; i++) {
    const p = addParticle(x + i * gap, y + Math.sin(i * 0.6) * 8, i === 0);
    if (previous) addConstraint(previous, p, gap);
    previous = p;
  }
}

function spawnCloth(x, y) {
  const cols = 18;
  const rows = 12;
  const gap = 22;
  const grid = [];

  for (let row = 0; row < rows; row++) {
    grid[row] = [];

    for (let col = 0; col < cols; col++) {
      const pinned = row === 0 && col % 3 === 0;
      const p = addParticle(x + col * gap, y + row * gap, pinned);
      grid[row][col] = p;

      if (col > 0) addConstraint(grid[row][col - 1], p, gap);
      if (row > 0) addConstraint(grid[row - 1][col], p, gap);

      if (row > 0 && col > 0) {
        addConstraint(grid[row - 1][col - 1], p, Math.sqrt(gap * gap * 2), 0.35);
      }

      if (row > 0 && col < cols - 1) {
        addConstraint(grid[row - 1][col + 1], p, Math.sqrt(gap * gap * 2), 0.35);
      }
    }
  }
}

function spawnBlob(x, y) {
  const center = addParticle(x, y, false);
  const ring = [];
  const count = 18;
  const radius = 78;

  for (let i = 0; i < count; i++) {
    const angle = (Math.PI * 2 * i) / count;
    const p = addParticle(
      x + Math.cos(angle) * radius,
      y + Math.sin(angle) * radius,
      false
    );

    ring.push(p);
    addConstraint(center, p, radius, 0.75);
  }

  for (let i = 0; i < count; i++) {
    addConstraint(ring[i], ring[(i + 1) % count], distance(ring[i], ring[(i + 1) % count]), 1);
    addConstraint(ring[i], ring[(i + 2) % count], distance(ring[i], ring[(i + 2) % count]), 0.35);
  }
}

function resetScene() {
  clearWorld();
  spawnCloth(120, 70);
  spawnRope(650, 90);
  spawnBlob(760, 360);
}

function findNearestParticle(x, y, maxDist = SETTINGS.grabRadius) {
  let nearest = null;
  let best = maxDist;

  for (const p of particles) {
    const d = Math.hypot(p.x - x, p.y - y);

    if (d < best) {
      nearest = p;
      best = d;
    }
  }

  return nearest;
}

function cutConstraints(x, y) {
  constraints = constraints.filter((c) => {
    const d = distancePointToSegment(x, y, c.a.x, c.a.y, c.b.x, c.b.y);
    return d > SETTINGS.cutRadius;
  });
}

function distancePointToSegment(px, py, x1, y1, x2, y2) {
  const dx = x2 - x1;
  const dy = y2 - y1;

  if (dx === 0 && dy === 0) {
    return Math.hypot(px - x1, py - y1);
  }

  const t = Math.max(0, Math.min(1, ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy)));
  const x = x1 + t * dx;
  const y = y1 + t * dy;

  return Math.hypot(px - x, py - y);
}

function updateStatuses() {
  gravityStatus.textContent = `Gravity: ${gravityEnabled ? "On" : "Off"}`;
  windStatus.textContent = `Wind: ${windEnabled ? "On" : "Off"}`;
  tearStatus.textContent = `Tearing: ${tearingEnabled ? "On" : "Off"}`;
}

function resizeCanvasToDisplaySize() {
  const rect = canvas.getBoundingClientRect();
  const dpr = window.devicePixelRatio || 1;

  const newWidth = Math.floor(rect.width * dpr);
  const newHeight = Math.floor(rect.height * dpr);

  if (canvas.width !== newWidth || canvas.height !== newHeight) {
    canvas.width = newWidth;
    canvas.height = newHeight;
    width = newWidth;
    height = newHeight;
    ctx.setTransform(1, 0, 0, 1, 0, 0);
  }
}

function getMousePos(event) {
  const rect = canvas.getBoundingClientRect();
  const scaleX = canvas.width / rect.width;
  const scaleY = canvas.height / rect.height;

  return {
    x: (event.clientX - rect.left) * scaleX,
    y: (event.clientY - rect.top) * scaleY
  };
}

canvas.addEventListener("contextmenu", (event) => event.preventDefault());

canvas.addEventListener("mousedown", (event) => {
  const pos = getMousePos(event);

  mouse.x = pos.x;
  mouse.y = pos.y;
  mouse.down = true;
  mouse.button = event.button;

  if (event.button === 0) {
    mouse.grabbed = findNearestParticle(mouse.x, mouse.y);
  }

  if (event.button === 1) {
    cutConstraints(mouse.x, mouse.y);
  }

  if (event.button === 2) {
    const p = findNearestParticle(mouse.x, mouse.y);
    if (p) {
      p.pinned = !p.pinned;
      p.oldX = p.x;
      p.oldY = p.y;
    }
  }
});

canvas.addEventListener("mousemove", (event) => {
  const pos = getMousePos(event);

  mouse.x = pos.x;
  mouse.y = pos.y;

  if (mouse.down && mouse.button === 1) {
    cutConstraints(mouse.x, mouse.y);
  }
});

window.addEventListener("mouseup", () => {
  mouse.down = false;
  mouse.grabbed = null;
});

window.addEventListener("keydown", (event) => {
  const tag = document.activeElement?.tagName?.toLowerCase();
  if (tag === "input" || tag === "textarea") return;

  if (event.code === "Space") {
    event.preventDefault();
    spawnRope(mouse.x, mouse.y);
  }

  if (event.key.toLowerCase() === "c") {
    spawnCloth(mouse.x - 160, mouse.y - 60);
  }

  if (event.key.toLowerCase() === "b") {
    spawnBlob(mouse.x, mouse.y);
  }

  if (event.key.toLowerCase() === "w") {
    windEnabled = !windEnabled;
    updateStatuses();
  }

  if (event.key.toLowerCase() === "t") {
    tearingEnabled = !tearingEnabled;
    updateStatuses();
  }

  if (event.key.toLowerCase() === "g") {
    gravityEnabled = !gravityEnabled;
    updateStatuses();
  }

  if (event.key.toLowerCase() === "r") {
    resetScene();
  }

  if (event.key === "Backspace" || event.key === "Delete") {
    clearWorld();
  }
});

function update() {
  resizeCanvasToDisplaySize();

  if (mouse.grabbed) {
    mouse.grabbed.x = mouse.x;
    mouse.grabbed.y = mouse.y;
  }

  for (const p of particles) {
    p.update();
  }

  for (let i = 0; i < SETTINGS.constraintIterations; i++) {
    constraints = constraints.filter((c) => c.solve());

    for (const p of particles) {
      p.constrainToBounds();
    }
  }
}

function drawBackground() {
  ctx.fillStyle = "#080a0f";
  ctx.fillRect(0, 0, width, height);

  ctx.save();
  ctx.globalAlpha = 0.18;
  ctx.strokeStyle = "#303747";
  ctx.lineWidth = 1;

  const grid = 42;
  for (let x = 0; x < width; x += grid) {
    ctx.beginPath();
    ctx.moveTo(x, 0);
    ctx.lineTo(x, height);
    ctx.stroke();
  }

  for (let y = 0; y < height; y += grid) {
    ctx.beginPath();
    ctx.moveTo(0, y);
    ctx.lineTo(width, y);
    ctx.stroke();
  }

  ctx.restore();
}

function drawHud() {
  ctx.fillStyle = "rgba(245, 245, 247, 0.78)";
  ctx.font = `${14 * (window.devicePixelRatio || 1)}px system-ui`;
  ctx.fillText(`Particles: ${particles.length}`, 18, 28);
  ctx.fillText(`Constraints: ${constraints.length}`, 18, 52);

  if (mouse.button === 1 && mouse.down) {
    ctx.beginPath();
    ctx.arc(mouse.x, mouse.y, SETTINGS.cutRadius, 0, Math.PI * 2);
    ctx.strokeStyle = "#f92672";
    ctx.lineWidth = 2;
    ctx.stroke();
  }
}

function draw() {
  drawBackground();

  for (const c of constraints) {
    c.draw();
  }

  for (const p of particles) {
    p.draw();
  }

  if (mouse.grabbed) {
    ctx.beginPath();
    ctx.arc(mouse.grabbed.x, mouse.grabbed.y, 15, 0, Math.PI * 2);
    ctx.strokeStyle = "#ffc900";
    ctx.lineWidth = 2;
    ctx.stroke();
  }

  drawHud();
}

function loop() {
  update();
  draw();
  requestAnimationFrame(loop);
}

updateStatuses();
resetScene();
loop();
