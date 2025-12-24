export function clamp(v: number, lo: number, hi: number): number {
  return Math.max(lo, Math.min(hi, v));
}

export function nicePadding(min: number, max: number, factor: number): [number, number] {
  if (!Number.isFinite(min) || !Number.isFinite(max)) return [min, max];
  if (min === max) {
    const pad = min === 0 ? 1 : Math.abs(min) * factor;
    return [min - pad, max + pad];
  }
  const span = max - min;
  const pad = span * factor;
  return [min - pad, max + pad];
}

export function formatNumber(v: number): string {
  if (!Number.isFinite(v)) return String(v);
  const av = Math.abs(v);
  if (av !== 0 && (av < 1e-6 || av >= 1e6)) return v.toExponential(6);
  return v.toFixed(12).replace(/0+$/, '').replace(/\.$/, '');
}
