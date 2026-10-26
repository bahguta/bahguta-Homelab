import { AfterViewInit, Component, ElementRef, OnDestroy, inject, input, viewChild } from '@angular/core';
import { WeatherScene } from '../../core/weather-icons';

const VERT = `attribute vec2 aPos; void main(){ gl_Position = vec4(aPos, 0.0, 1.0); }`;

/**
 * Cielo procedural: nubes con ruido fractal (3 capas, iluminadas), lluvia en 3 planos
 * de profundidad y destellos de rayo que iluminan las nubes.
 */
const FRAG = `
precision highp float;
uniform vec2 uRes;
uniform float uTime, uCover, uRain, uFlash, uFlashX, uNight, uSize, uPosX, uPosY;

float hash(vec2 p){ vec3 q = fract(vec3(p.xyx) * 0.1031); q += dot(q, q.yzx + 33.33); return fract((q.x + q.y) * q.z); }
float noise(vec2 p){
  vec2 i = floor(p), f = fract(p); f = f * f * (3.0 - 2.0 * f);
  return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}
float fbm(vec2 p){
  float v = 0.0, a = 0.5;
  for (int i = 0; i < 5; i++) { v += a * noise(p); p = p * 2.02 + vec2(11.7, 3.1); a *= 0.5; }
  return v;
}
float cloud(vec2 p, float thr, float t){
  float q = fbm(p * 0.7 + vec2(t * 0.015, 0.0));
  float f = fbm(p + vec2(q * 1.8 + t * 0.04, q * 0.6));
  return smoothstep(thr, thr + 0.42, f);
}
float rain(vec2 uv, float scale, float speed, float seed, float asp){
  vec2 p = vec2(uv.x * asp, uv.y);
  p.x += p.y * 0.35;
  p *= vec2(scale, scale * 0.07);
  // Cada columna cae a su ritmo y con su propia fase: así no se forman filas alineadas
  float col = floor(p.x);
  p.y += uTime * speed * (0.75 + 0.5 * hash(vec2(col, seed + 2.0))) + hash(vec2(col, seed + 1.0)) * 40.0;
  vec2 id = floor(p), f = fract(p);
  float on = step(0.70, hash(id + seed + 7.3));
  float x = 0.15 + 0.7 * hash(id + seed);
  float y0 = 0.1 + 0.5 * hash(id + seed + 11.0);   // posición de la cabeza de la gota en su celda
  float len = 0.25 + 0.3 * hash(id + seed + 5.0);   // largo de la estela
  float dy = f.y - y0;
  float line = smoothstep(0.10, 0.0, abs(f.x - x));
  float tail = step(0.0, dy) * smoothstep(len, 0.0, dy);
  return line * tail * on;
}

void main(){
  vec2 uv = gl_FragCoord.xy / uRes;
  float asp = uRes.x / uRes.y;
  vec2 p = vec2(uv.x * asp, uv.y);

  // Cielo: de día, azul que se apaga hacia gris con la cobertura; de noche, oscurecido por las nubes
  float day = 1.0 - uNight;
  float skyA = mix(0.62 * uCover * uCover, mix(0.82, 0.62, uCover), day);
  vec3 daySky = mix(vec3(0.20, 0.48, 0.86), vec3(0.16, 0.21, 0.31), smoothstep(0.25, 0.9, uCover));
  daySky *= 1.0 + 0.25 * smoothstep(0.9, 0.0, uv.y) * (1.0 - uCover);
  vec3 sky = mix(vec3(0.07, 0.09, 0.17), daySky, day);
  vec3 c = sky * skyA;
  float a = skyA;

  // Brillo del rayo, localizado alrededor de su posición horizontal
  float gx = (uv.x - uFlashX) * asp;
  float glow = exp(-gx * gx * 1.6);
  vec3 flashCol = vec3(0.55, 0.65, 1.0);
  c += flashCol * uFlash * 0.18 * (0.3 + glow) * a;

  // Luna y estrellas: de noche, con cualquier tiempo
  vec2 mp = p - vec2(uPosX * asp, uPosY);
  float R = uSize;
  float md = length(mp) / R;
  float moonMask = 0.0;
  if (uNight > 0.5) {
    vec2 sc = vec2(uv.x * asp, uv.y) * 90.0;
    vec2 sg = floor(sc);
    float star = step(0.992, hash(sg)) * (0.6 + 0.4 * sin(uTime * 2.0 + hash(sg + 3.0) * 40.0));
    star *= smoothstep(0.45, 0.05, length(fract(sc) - 0.5));
    star *= (1.0 - clamp(uCover * 1.5, 0.0, 1.0)) * smoothstep(0.2, 0.6, uv.y);
    c += vec3(0.9, 0.93, 1.0) * star;
    a = max(a, star);

    float disc = smoothstep(1.0, 0.96, md);
    float tex = fbm(mp / R * 2.2 + 4.0);
    vec3 moonCol = mix(vec3(0.74, 0.78, 0.86), vec3(0.97, 0.97, 1.0), smoothstep(0.35, 0.7, tex));
    moonCol *= 0.86 + 0.14 * sqrt(max(1.0 - md * md, 0.0));
    moonCol *= mix(0.55, 1.0, smoothstep(-1.0, 0.7, -mp.x / R * 0.8 + mp.y / R * 0.25));
    float halo = (exp(-max(md - 1.0, 0.0) * 2.6) * 0.30 + exp(-max(md - 1.0, 0.0) * 0.7) * 0.10) * (1.0 - disc);
    c += vec3(0.55, 0.65, 1.0) * halo;
    a = a + halo * 0.7 * (1.0 - a);
    c = c * (1.0 - disc) + moonCol * disc;
    a = disc + a * (1.0 - disc);
    moonMask = exp(-md * md * 0.7);
  }

  // Sol: de día; se vela con las nubes y casi desaparece con lluvia
  float sunMask = 0.0;
  if (uNight < 0.5) {
    float vis = clamp(1.3 - 1.4 * uCover, 0.12, 1.0) * (1.0 - 0.8 * uRain);
    float ang = atan(mp.y, mp.x);
    float rays = (pow(abs(sin(ang * 7.0 + uTime * 0.12)), 18.0) + 0.6 * pow(abs(sin(ang * 13.0 - uTime * 0.08)), 30.0)) * exp(-md * 0.55);
    float glowS = exp(-md * 1.2) * 0.65 + exp(-md * 0.3) * 0.22;
    float L = (glowS * 0.85 + rays * 0.30) * vis;
    c += vec3(1.0, 0.93, 0.70) * L;
    a = a + L * 0.6 * (1.0 - a);
    float core = smoothstep(1.0, 0.82, md) * vis;
    c = c * (1.0 - core) + vec3(1.0, 0.98, 0.88) * core;
    a = core + a * (1.0 - core);
    sunMask = exp(-md * md * 0.6) * vis;
  }

  float thr = mix(0.78, 0.30, uCover);
  vec2 ld = vec2(-0.05, 0.08);

  for (int i = 0; i < 3; i++) {
    float fi = float(i);
    float scale = 0.8 + fi * 0.7;
    float tt = uTime * (1.0 + fi * 0.8);
    vec2 q = p * scale + vec2(fi * 13.0, fi * 5.0);
    float d0 = cloud(q, thr + 0.04 * fi, tt);

    // Iluminación: la nube es más clara donde la densidad baja hacia la luz
    float lit = 0.5;
    if (i > 0) {
      float d1 = cloud(q + ld * scale, thr + 0.04 * fi, tt);
      lit = clamp(0.55 + (d0 - d1) * 1.8, 0.0, 1.0);
    }

    float mask = mix(1.0, smoothstep(0.05, 0.95, uv.y + 0.25), 0.75 * uCover);
    float la = d0 * mask * (0.55 + 0.2 * fi) * clamp(uCover * 1.4, 0.0, 1.0);
    la *= 1.0 - 0.5 * max(moonMask, sunMask * 0.9);

    vec3 dark = mix(vec3(0.16, 0.19, 0.27), vec3(0.08, 0.10, 0.18), uNight);
    vec3 light = mix(vec3(0.86, 0.89, 0.95), vec3(0.34, 0.40, 0.55), uNight);
    vec3 col = mix(dark, light, lit) * (1.0 - 0.32 * uRain);
    col += vec3(0.60, 0.70, 1.0) * moonMask * 0.30 * (0.4 + 0.6 * d0);
    col += vec3(1.0, 0.85, 0.60) * sunMask * 0.35 * (0.4 + 0.6 * d0);
    col += flashCol * uFlash * (0.25 + glow * (0.4 + uv.y)) * (0.4 + 0.6 * d0);

    c = c * (1.0 - la) + col * la;
    a = la + a * (1.0 - la);
  }

  // Lluvia
  float r = rain(uv, 26.0, 3.0, 1.0, asp) * 0.35
          + rain(uv, 42.0, 4.2, 5.0, asp) * 0.50
          + rain(uv, 68.0, 5.8, 9.0, asp) * 0.70;
  r *= uRain;
  c += vec3(0.70, 0.78, 0.95) * (1.0 + uFlash * 1.5) * r * 0.7;
  a = a + r * 0.5 * (1.0 - a);

  c = min(c, vec3(a));
  gl_FragColor = vec4(c, a);
}`;

interface Seg { x1: number; y1: number; x2: number; y2: number; w: number }

/** Intensidad del destello en función del tiempo desde el rayo (s): varios parpadeos y caída. */
const envelope = (t: number) =>
  t < 0.07 ? 1 : t < 0.12 ? 0.2 : t < 0.19 ? 0.9 : t < 0.26 ? 0.15 : t < 0.32 ? 0.7 : Math.exp(-(t - 0.32) * 7);

@Component({
  selector: 'app-weather-bg',
  template: `<canvas #gl class="gl"></canvas><canvas #fx class="fx"></canvas>`,
  styles: `
    :host { position: absolute; inset: 0; z-index: 0; overflow: hidden; border-radius: inherit; pointer-events: none; }
    canvas { position: absolute; inset: 0; width: 100%; height: 100%; }
  `,
})
export class WeatherBg implements AfterViewInit, OnDestroy {
  readonly scene = input.required<WeatherScene>();
  /** Radio del sol / la luna, relativo al alto del fondo. */
  readonly size = input(0.13);
  /** Posición del sol / la luna (x, y) en fracciones del fondo; y crece hacia arriba. */
  readonly pos = input<[number, number]>([0.8, 0.62]);

  private readonly glCanvas = viewChild.required<ElementRef<HTMLCanvasElement>>('gl');
  private readonly fxCanvas = viewChild.required<ElementRef<HTMLCanvasElement>>('fx');
  private readonly host = inject<ElementRef<HTMLElement>>(ElementRef);

  private gl?: WebGLRenderingContext;
  private loc: Record<string, WebGLUniformLocation | null> = {};
  private fx?: CanvasRenderingContext2D;
  private raf = 0;
  private ro?: ResizeObserver;
  private io?: IntersectionObserver;
  private visible = true;
  private readonly reduced = matchMedia('(prefers-reduced-motion: reduce)').matches;
  private readonly t0 = performance.now() - Math.random() * 60_000;

  // Rayos
  private nextStrike = performance.now() + 1500;
  private strike?: { start: number; segs: Seg[] | null; power: number; x: number };
  private fxDirty = false;
  private w = 1;
  private h = 1;

  ngAfterViewInit(): void {
    this.initGl();
    this.fx = this.fxCanvas().nativeElement.getContext('2d') ?? undefined;

    const el = this.host.nativeElement;
    this.ro = new ResizeObserver(() => this.resize());
    this.ro.observe(el);
    this.io = new IntersectionObserver(([e]) => (this.visible = e.isIntersecting));
    this.io.observe(el);

    this.resize();
    this.raf = requestAnimationFrame(this.frame);
  }

  ngOnDestroy(): void {
    cancelAnimationFrame(this.raf);
    this.ro?.disconnect();
    this.io?.disconnect();
    this.gl?.getExtension('WEBGL_lose_context')?.loseContext();
  }

  // ---------------- WebGL ----------------
  private initGl(): void {
    const gl = this.glCanvas().nativeElement.getContext('webgl', { premultipliedAlpha: true, alpha: true, antialias: false });
    if (!gl) return;
    const sh = (type: number, src: string) => {
      const s = gl.createShader(type)!;
      gl.shaderSource(s, src);
      gl.compileShader(s);
      if (!gl.getShaderParameter(s, gl.COMPILE_STATUS)) console.error(gl.getShaderInfoLog(s));
      return s;
    };
    const prog = gl.createProgram()!;
    gl.attachShader(prog, sh(gl.VERTEX_SHADER, VERT));
    gl.attachShader(prog, sh(gl.FRAGMENT_SHADER, FRAG));
    gl.linkProgram(prog);
    if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) return;
    gl.useProgram(prog);

    const buf = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buf);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([-1, -1, 1, -1, -1, 1, 1, 1]), gl.STATIC_DRAW);
    const a = gl.getAttribLocation(prog, 'aPos');
    gl.enableVertexAttribArray(a);
    gl.vertexAttribPointer(a, 2, gl.FLOAT, false, 0, 0);

    for (const n of ['uRes', 'uTime', 'uCover', 'uRain', 'uFlash', 'uFlashX', 'uNight', 'uSize', 'uPosX', 'uPosY']) {
      this.loc[n] = gl.getUniformLocation(prog, n);
    }
    gl.clearColor(0, 0, 0, 0);
    this.gl = gl;
  }

  private resize(): void {
    const el = this.host.nativeElement;
    this.w = Math.max(1, el.clientWidth);
    this.h = Math.max(1, el.clientHeight);

    // El cielo se calcula a menor resolución (es difuso) para ir fluido.
    const g = this.glCanvas().nativeElement;
    g.width = Math.max(1, Math.round(this.w * 0.7));
    g.height = Math.max(1, Math.round(this.h * 0.7));
    this.gl?.viewport(0, 0, g.width, g.height);

    const k = Math.min(window.devicePixelRatio || 1, 1.5);
    const f = this.fxCanvas().nativeElement;
    f.width = Math.round(this.w * k);
    f.height = Math.round(this.h * k);
    this.fx?.setTransform(k, 0, 0, k, 0, 0);

    if (this.reduced) this.render(performance.now());
  }

  // ---------------- bucle ----------------
  private readonly frame = (now: number) => {
    if (!this.reduced) this.raf = requestAnimationFrame(this.frame);
    if (this.visible) this.render(now);
  };

  private render(now: number): void {
    const s = this.scene();
    const flash = this.updateLightning(now, s);

    const gl = this.gl;
    if (gl) {
      const t = this.reduced ? 12 : (now - this.t0) / 1000;
      const g = this.glCanvas().nativeElement;
      gl.uniform2f(this.loc['uRes'], g.width, g.height);
      gl.uniform1f(this.loc['uTime'], t);
      gl.uniform1f(this.loc['uCover'], s.cover);
      gl.uniform1f(this.loc['uRain'], s.rain);
      gl.uniform1f(this.loc['uNight'], s.night ? 1 : 0);
      gl.uniform1f(this.loc['uSize'], this.size());
      gl.uniform1f(this.loc['uPosX'], this.pos()[0]);
      gl.uniform1f(this.loc['uPosY'], this.pos()[1]);
      gl.uniform1f(this.loc['uFlash'], flash);
      gl.uniform1f(this.loc['uFlashX'], this.strike?.x ?? 0.5);
      gl.clear(gl.COLOR_BUFFER_BIT);
      gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
    }
  }

  // ---------------- rayos ----------------
  /** Programa y anima los rayos; devuelve la intensidad actual del destello (0–1). */
  private updateLightning(now: number, s: WeatherScene): number {
    if (!s.lightning || this.reduced) return 0;

    if (!this.strike && now >= this.nextStrike) {
      const visibleBolt = Math.random() < 0.65;
      const x = 0.12 + Math.random() * 0.76;
      this.strike = {
        start: now,
        segs: visibleBolt ? this.makeBolt(x) : null,
        power: visibleBolt ? 1 : 0.45,
        x,
      };
      // A veces cae un segundo rayo casi enseguida
      this.nextStrike = now + (Math.random() < 0.25 ? 350 + Math.random() * 300 : 2200 + Math.random() * 5500);
    }

    const st = this.strike;
    if (!st) return 0;

    const t = (now - st.start) / 1000;
    if (t > 1.3) {
      this.strike = undefined;
      this.clearFx();
      return 0;
    }

    const e = envelope(t) * st.power;
    if (st.segs) this.drawBolt(st.segs, e);
    return e;
  }

  private makeBolt(xNorm: number): Seg[] {
    const segs: Seg[] = [];
    const x0 = this.w * xNorm;
    const x1 = x0 + (Math.random() - 0.5) * this.w * 0.2;
    const y1 = this.h * (0.55 + Math.random() * 0.35);
    this.split(segs, x0, -10, x1, y1, this.h * 0.14, 1, 0);
    return segs;
  }

  private split(out: Seg[], x1: number, y1: number, x2: number, y2: number, disp: number, w: number, depth: number): void {
    if (Math.hypot(x2 - x1, y2 - y1) < 9) {
      out.push({ x1, y1, x2, y2, w });
      return;
    }
    const mx = (x1 + x2) / 2 + (Math.random() - 0.5) * disp;
    const my = (y1 + y2) / 2 + (Math.random() - 0.5) * disp * 0.4;
    this.split(out, x1, y1, mx, my, disp / 2, w, depth);
    this.split(out, mx, my, x2, y2, disp / 2, w, depth);

    // Ramificaciones, más finas
    if (depth < 2 && disp > 12 && Math.random() < 0.3) {
      const dir = Math.random() < 0.5 ? -1 : 1;
      const len = (y2 - y1) * (0.6 + Math.random() * 0.8);
      this.split(out, mx, my, mx + dir * len * (0.3 + Math.random() * 0.5), my + len, disp / 2, w * 0.55, depth + 1);
    }
  }

  private drawBolt(segs: Seg[], e: number): void {
    const ctx = this.fx;
    if (!ctx) return;
    ctx.clearRect(0, 0, this.w, this.h);
    ctx.globalCompositeOperation = 'lighter';
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';

    const passes: [number, string][] = [
      [16, `rgba(90,120,255,${0.14 * e})`],
      [6, `rgba(150,175,255,${0.40 * e})`],
      [2, `rgba(255,255,255,${0.98 * e})`],
    ];
    for (const [width, color] of passes) {
      ctx.strokeStyle = color;
      for (const s of segs) {
        ctx.lineWidth = width * s.w;
        ctx.beginPath();
        ctx.moveTo(s.x1, s.y1);
        ctx.lineTo(s.x2, s.y2);
        ctx.stroke();
      }
    }
    this.fxDirty = true;
  }

  private clearFx(): void {
    if (this.fx && this.fxDirty) {
      this.fx.clearRect(0, 0, this.w, this.h);
      this.fxDirty = false;
    }
  }
}
