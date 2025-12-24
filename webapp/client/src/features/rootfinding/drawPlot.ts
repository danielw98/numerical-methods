import { evalEquation } from './equations';
import { formatNumber } from './number';
import type { EqId, RootFindingMethodName, RootFindingTraceStep } from './types';
import type { PlotDerived } from './plotDerived';

type Theme = {
  bg: string;
  text: string;
  muted: string;
  border2: string;
  accent: string;
  accent2: string;
};

function readTheme(): Theme {
  const rootStyle = getComputedStyle(document.documentElement);
  return {
    bg: rootStyle.getPropertyValue('--surface').trim() || '#0f172a',
    text: rootStyle.getPropertyValue('--text').trim() || '#e5e7eb',
    muted: rootStyle.getPropertyValue('--muted').trim() || 'rgba(229, 231, 235, 0.72)',
    border2: rootStyle.getPropertyValue('--border2').trim() || 'rgba(255, 255, 255, 0.18)',
    accent: rootStyle.getPropertyValue('--brand').trim() || '#7c3aed',
    accent2: rootStyle.getPropertyValue('--brand2').trim() || '#22c55e',
  };
}

function trimTrailingZerosFixed(s: string): string {
  // s is expected to be something like "12.340000" or "12".
  const dot = s.indexOf('.');
  if (dot < 0) return s;
  let end = s.length;
  while (end > dot + 1 && s[end - 1] === '0') end--;
  if (end === dot + 1) end = dot; // remove trailing dot
  return s.slice(0, end);
}

function formatTick(v: number): string {
  if (!Number.isFinite(v)) return String(v);
  const av = Math.abs(v);
  if (av !== 0 && (av < 1e-5 || av >= 1e5)) return v.toExponential(3);
  return trimTrailingZerosFixed(v.toFixed(4));
}

type LegendEntry = {
  label: string;
  value?: string;
  color: string;
};

function drawLegend(ctx: CanvasRenderingContext2D, theme: Theme, plot: { x: number; y: number; w: number; h: number }, entries: LegendEntry[]) {
  if (entries.length === 0) return;

  const font = '12px system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif';
  ctx.save();
  ctx.font = font;
  ctx.textBaseline = 'alphabetic';
  ctx.textAlign = 'left';

  const dotR = 4;
  const pad = 10;
  const lineH = 16;
  const dotGap = 8;
  const valueGap = 10;

  let maxW = 0;
  for (const e of entries) {
    const s = e.value ? `${e.label}${valueGap ? '' : ''}` : e.label;
    const labelW = ctx.measureText(s).width;
    const valueW = e.value ? ctx.measureText(e.value).width : 0;
    maxW = Math.max(maxW, labelW + (e.value ? valueGap + valueW : 0));
  }

  const boxW = pad * 2 + dotR * 2 + dotGap + maxW;
  const boxH = pad * 2 + entries.length * lineH;

  // Top-right inside plot area
  let bx = plot.x + plot.w - boxW - 8;
  let by = plot.y + 8;
  if (bx < plot.x + 2) bx = plot.x + 2;
  if (by < plot.y + 2) by = plot.y + 2;

  ctx.globalAlpha = 0.88;
  ctx.fillStyle = theme.bg;
  ctx.strokeStyle = theme.border2;
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.rect(bx, by, boxW, boxH);
  ctx.fill();
  ctx.stroke();

  ctx.globalAlpha = 1;
  for (let i = 0; i < entries.length; i++) {
    const e = entries[i];
    const cy = by + pad + i * lineH + 10;
    const cx = bx + pad + dotR;

    ctx.fillStyle = e.color;
    ctx.beginPath();
    ctx.arc(cx, cy - 4, dotR, 0, Math.PI * 2);
    ctx.fill();

    const tx = cx + dotR + dotGap;
    ctx.fillStyle = theme.text;
    ctx.fillText(e.label, tx, cy);

    if (e.value) {
      const lw = ctx.measureText(e.label).width;
      ctx.fillStyle = theme.text;
      ctx.fillText(e.value, tx + lw + valueGap, cy);
    }
  }

  ctx.restore();
}

export function drawRootFindingPlot(params: {
  canvas: HTMLCanvasElement;
  hostWidth: number;
  isVizLayout: boolean;
  derived: PlotDerived;
  step: RootFindingTraceStep | undefined;
  method: RootFindingMethodName;
  eq: EqId;
}) {
  const dpr = window.devicePixelRatio || 1;
  const w = Math.max(280, Math.floor(params.hostWidth));
  let h = params.isVizLayout ? 520 : 360;
  if (params.isVizLayout && w < 520) h = 420;

  params.canvas.width = Math.floor(w * dpr);
  params.canvas.height = Math.floor(h * dpr);
  params.canvas.style.width = `${w}px`;
  params.canvas.style.height = `${h}px`;

  const ctx = params.canvas.getContext('2d');
  if (!ctx) return;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);

  const theme = readTheme();
  const grid = 'rgba(255, 255, 255, 0.10)';
  const curve = 'rgba(229, 231, 235, 0.86)';

  ctx.clearRect(0, 0, w, h);
  ctx.fillStyle = theme.bg;
  ctx.fillRect(0, 0, w, h);

  const padL = 44;
  const padR = 14;
  const padT = 14;
  const padB = 34;
  const plotW = w - padL - padR;
  const plotH = h - padT - padB;

  const xToPx = (x: number) => padL + ((x - params.derived.xMin) / (params.derived.xMax - params.derived.xMin)) * plotW;
  const yToPx = (y: number) => padT + (1 - (y - params.derived.yMin) / (params.derived.yMax - params.derived.yMin)) * plotH;

  const clampPxX = (x: number) => Math.max(padL + 2, Math.min(padL + plotW - 2, x));
  const clampPxY = (y: number) => Math.max(padT + 2, Math.min(padT + plotH - 2, y));

  const colors = {
    a: theme.accent2,
    b: theme.text,
    p: theme.accent,
    pk: theme.text,
    pk1: theme.accent2,
  };

  // grid
  ctx.strokeStyle = grid;
  ctx.lineWidth = 1;
  for (let i = 0; i <= 4; i++) {
    const yy = padT + (plotH * i) / 4;
    ctx.beginPath();
    ctx.moveTo(padL, yy);
    ctx.lineTo(padL + plotW, yy);
    ctx.stroke();
  }
  for (let i = 0; i <= 4; i++) {
    const xx = padL + (plotW * i) / 4;
    ctx.beginPath();
    ctx.moveTo(xx, padT);
    ctx.lineTo(xx, padT + plotH);
    ctx.stroke();
  }

  // axes y=0
  const y0 = yToPx(0);
  ctx.strokeStyle = theme.border2;
  ctx.lineWidth = 1.5;
  ctx.beginPath();
  ctx.moveTo(padL, y0);
  ctx.lineTo(padL + plotW, y0);
  ctx.stroke();

  // curve
  ctx.strokeStyle = curve;
  ctx.lineWidth = 2.6;
  ctx.beginPath();
  let started = false;
  for (const p of params.derived.pts) {
    const xx = xToPx(p.x);
    const yy = yToPx(p.y);
    if (!started) {
      ctx.moveTo(xx, yy);
      started = true;
    } else {
      ctx.lineTo(xx, yy);
    }
  }
  ctx.stroke();
  ctx.setLineDash([]);

  // overlay
  const step = params.step;
  if (step) {
    const dot = (x: number, y: number, color: string) => {
      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.arc(x, y, 5.2, 0, Math.PI * 2);
      ctx.fill();
    };

    const line = (x1: number, y1: number, x2: number, y2: number, color: string, width = 2.2) => {
      ctx.strokeStyle = color;
      ctx.lineWidth = width;
      ctx.beginPath();
      ctx.moveTo(x1, y1);
      ctx.lineTo(x2, y2);
      ctx.stroke();
    };

    const vlineAtX = (x: number, color: string) => {
      const xx = xToPx(x);
      line(xx, padT, xx, padT + plotH, color, 1.5);
    };

    const legend: LegendEntry[] = [];

    if ('p' in step) {
      const px = xToPx(step.p);
      const py = yToPx(step.fp);
      vlineAtX(step.p, 'rgba(124, 58, 237, 0.28)');
      dot(px, py, colors.p);
      dot(px, y0, colors.p);

      legend.push({ label: 'p', value: `f(p)=${formatTick(step.fp)}`, color: colors.p });
    }

    if (params.method === 'newton' && 'x' in step) {
      const x = step.x;
      const fx = step.fx;
      const dfx = step.dfx;
      const xNext = step.xNext;

      const xPx = xToPx(x);
      const fxPx = yToPx(fx);
      dot(xPx, fxPx, colors.pk);

      legend.push({ label: 'p_k', value: `f(p_k)=${formatTick(fx)}`, color: colors.pk });

      const t1 = params.derived.xMin;
      const t2 = params.derived.xMax;
      const y1 = fx + dfx * (t1 - x);
      const y2 = fx + dfx * (t2 - x);
      ctx.setLineDash([4, 4]);
      line(xToPx(t1), yToPx(y1), xToPx(t2), yToPx(y2), theme.accent2, 2.2);
      ctx.setLineDash([]);

      vlineAtX(xNext, 'rgba(34, 197, 94, 0.32)');
      dot(xToPx(xNext), y0, colors.pk1);

      legend.push({ label: 'p_{k+1}', value: `=${formatTick(xNext)}`, color: colors.pk1 });
    }

    if ((params.method === 'regulaFalsi' || params.method === 'bisection') && 'a' in step) {
      dot(xToPx(step.a), y0, colors.a);
      dot(xToPx(step.b), y0, colors.b);

      const fa = evalEquation(params.eq, step.a);
      const fb = evalEquation(params.eq, step.b);
      if (Number.isFinite(fa)) legend.push({ label: 'a', value: `f(a)=${formatTick(fa)}`, color: colors.a });
      if (Number.isFinite(fb)) legend.push({ label: 'b', value: `f(b)=${formatTick(fb)}`, color: colors.b });

      if (params.method === 'regulaFalsi') {
        if (Number.isFinite(fa) && Number.isFinite(fb)) {
          ctx.setLineDash([4, 4]);
          line(xToPx(step.a), yToPx(fa), xToPx(step.b), yToPx(fb), 'rgba(255,255,255,0.32)', 2.2);
          ctx.setLineDash([]);
        }
      }
    }

    if (params.method === 'secant' && 'x0' in step) {
      const f0 = evalEquation(params.eq, step.x0);
      const f1 = evalEquation(params.eq, step.x1);
      if (Number.isFinite(f0) && Number.isFinite(f1)) {
        ctx.setLineDash([4, 4]);
        line(xToPx(step.x0), yToPx(f0), xToPx(step.x1), yToPx(f1), 'rgba(255,255,255,0.32)', 2.2);
        ctx.setLineDash([]);
        dot(xToPx(step.x0), yToPx(f0), colors.b);
        dot(xToPx(step.x1), yToPx(f1), colors.a);

        legend.push({ label: 'p_{k-1}', value: `f=${formatTick(f0)}`, color: colors.b });
        legend.push({ label: 'p_k', value: `f=${formatTick(f1)}`, color: colors.a });
        legend.push({ label: 'p', value: `=${formatTick(step.p)}`, color: colors.p });
      }
    }

    // Draw legend last so it stays readable and doesn't overlap other callouts.
    drawLegend(ctx, theme, { x: padL, y: padT, w: plotW, h: plotH }, legend);
  }

  // axis labels
  ctx.fillStyle = theme.text;
  ctx.font = '12px system-ui, -apple-system, Segoe UI, Roboto, Arial, sans-serif';
  ctx.textBaseline = 'alphabetic';
  ctx.fillText(formatTick(params.derived.xMin), padL, h - 12);
  const maxLabel = formatTick(params.derived.xMax);
  const tw = ctx.measureText(maxLabel).width;
  ctx.fillText(maxLabel, padL + plotW - tw, h - 12);

  // y labels
  ctx.fillStyle = theme.text;
  ctx.textBaseline = 'top';
  ctx.fillText(formatTick(params.derived.yMax), 8, padT);
  ctx.textBaseline = 'bottom';
  ctx.fillText(formatTick(params.derived.yMin), 8, padT + plotH);
  if (y0 >= padT && y0 <= padT + plotH) {
    ctx.textBaseline = 'alphabetic';
    ctx.fillText('0', 8, y0 + 4);
  }
}
