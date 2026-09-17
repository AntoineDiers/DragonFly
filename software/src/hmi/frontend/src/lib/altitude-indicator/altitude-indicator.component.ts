import { Component, Input } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FlightControllerStateOverview } from '../../structs/FlightControllerStateOverview';

@Component({
  selector: 'altitude-indicator',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './altitude-indicator.component.html',
  styleUrl: './altitude-indicator.component.css'
})
export class AltitudeIndicatorComponent {

    @Input() fc_state_overview : FlightControllerStateOverview = FlightControllerStateOverview.Default();

    protected readonly Math = Math;

    range = 40;
    step = 1;
    label_step = 5;

    altitudeMarks(): number[] {
        const count = Math.ceil(this.range / this.step);

        const centerAltitude =
        Math.round(this.fc_state_overview.navigation.altitude_m / this.step) * this.step;

        return Array.from(
        { length: count * 2 + 1 },
        (_, i) =>
            centerAltitude +
            (i - count) * this.step
        );
    }


    altitudeMarkPosPercent(value: number): number {
        return 50 - 50 * (value - this.fc_state_overview.navigation.altitude_m) / this.range;
    }
}