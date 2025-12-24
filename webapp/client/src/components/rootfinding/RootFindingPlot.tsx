import React, { useEffect, useRef } from 'react';
import { drawRootFindingPlot } from '../../features/rootfinding/drawPlot';
import type { EqId, RootFindingMethodName, RootFindingTraceStep } from '../../features/rootfinding/types';
import type { PlotDerived } from '../../features/rootfinding/plotDerived';

export function RootFindingPlot(props: {
  isVizLayout: boolean;
  derived: PlotDerived;
  step: RootFindingTraceStep | undefined;
  method: RootFindingMethodName;
  eq: EqId;
  loading: boolean;
}) {
  const hostRef = useRef<HTMLDivElement | null>(null);
  const canvasRef = useRef<HTMLCanvasElement | null>(null);

  useEffect(() => {
    const host = hostRef.current;
    const canvas = canvasRef.current;
    if (!host || !canvas) return;

    const ro = new ResizeObserver(() => {
      drawRootFindingPlot({
        canvas,
        hostWidth: host.clientWidth,
        isVizLayout: props.isVizLayout,
        derived: props.derived,
        step: props.step,
        method: props.method,
        eq: props.eq,
      });
    });

    ro.observe(host);

    drawRootFindingPlot({
      canvas,
      hostWidth: host.clientWidth,
      isVizLayout: props.isVizLayout,
      derived: props.derived,
      step: props.step,
      method: props.method,
      eq: props.eq,
    });

    return () => {
      ro.disconnect();
    };
  }, [props.isVizLayout, props.derived, props.step, props.method, props.eq]);

  return (
    <div className="nmGraph" ref={hostRef}>
      <canvas ref={canvasRef} />
      {props.loading ? <div className="nmGraphOverlay">Updating…</div> : null}
    </div>
  );
}
