import React from 'react';
import { EQUATIONS, clampEq, clampInterval } from '../../features/rootfinding/equations';
import type { EqId, IntervalId } from '../../features/rootfinding/types';

export function RootFindingHeaderControls(props: {
  eq: EqId;
  interval: IntervalId;
  setEq: (v: EqId) => void;
  setInterval: (v: IntervalId) => void;
  iter: number;
  canPrev: boolean;
  canNext: boolean;
  onPrev: () => void;
  onNext: () => void;
}) {
  return (
    <div className="nmWidgetControls nmWidgetControlsWrap">
      <label className="nmInlineField">
        <span>Ecuație</span>
        <select
          value={props.eq}
          onChange={(e) => props.setEq(clampEq(Number(e.target.value)))}
          className="nmSelect"
        >
          <option value={1}>(1) {EQUATIONS[1].short}</option>
          <option value={2}>(2) {EQUATIONS[2].short}</option>
          <option value={3}>(3) {EQUATIONS[3].short}</option>
          <option value={4}>(4) {EQUATIONS[4].short}</option>
        </select>
      </label>

      <label className="nmInlineField">
        <span>Interval</span>
        <select
          value={props.interval}
          onChange={(e) => props.setInterval(clampInterval(Number(e.target.value)))}
          className="nmSelect nmSelectSm"
        >
          <option value={1}>1</option>
          <option value={2}>2</option>
        </select>
      </label>

      <button type="button" className="nmBtn" onClick={props.onPrev} disabled={!props.canPrev}>
        Prev
      </button>
      <div className="nmWidgetStep">iter = {props.iter}</div>
      <button type="button" className="nmBtn" onClick={props.onNext} disabled={!props.canNext}>
        Next
      </button>
    </div>
  );
}
