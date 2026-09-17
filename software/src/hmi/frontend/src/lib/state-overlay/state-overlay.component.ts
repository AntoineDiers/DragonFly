import { Component, Input } from '@angular/core';
import { CommonModule } from '@angular/common';
import {  FlightControllerStateOverview, 
          ControlModes, 
          DiagLevel } from '../../structs/FlightControllerStateOverview';

@Component({
  selector: 'state-overlay',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './state-overlay.component.html',
  styleUrl: './state-overlay.component.css'
})
export class StateOverlayComponent {

    protected readonly ControlModes = ControlModes;

    @Input() fc_state_overview : FlightControllerStateOverview = FlightControllerStateOverview.Default();

    getInputDiagClass(level : DiagLevel) : 'input-unknown' | 'input-ok' | 'input-warn' | 'input-err'
    {
        if(level == DiagLevel.OK) { return 'input-ok'; }
        if(level == DiagLevel.WARN) { return 'input-warn'; }
        if(level == DiagLevel.ERR) { return 'input-err'; }
        return 'input-unknown';
    }
}