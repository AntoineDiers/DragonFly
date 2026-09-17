import { Component, Input } from '@angular/core';
import { CommonModule } from '@angular/common';
import { GcConnectionState, DiagLevel } from '../../structs/GcConnectionsState';

@Component({
  selector: 'connection-overlay',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './connection-overlay.component.html',
  styleUrl: './connection-overlay.component.css'
})
export class ConnectionOverlayComponent {

    @Input() gc_connection_state : GcConnectionState = GcConnectionState.Default();

    getDiagClass(level : DiagLevel) : 'diag-unknown' | 'diag-ok' | 'diag-warn' | 'diag-err'
    {
        if(level == DiagLevel.OK) { return 'diag-ok'; }
        if(level == DiagLevel.WARN) { return 'diag-warn'; }
        if(level == DiagLevel.ERR) { return 'diag-err'; }
        return 'diag-unknown';
    }
}