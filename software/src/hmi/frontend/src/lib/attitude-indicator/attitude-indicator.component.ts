import { Component, Input } from '@angular/core';
import { FlightControllerStateOverview } from '../../structs/FlightControllerStateOverview';

@Component({
  selector: 'attitude-indicator',
  standalone: true,
  templateUrl: './attitude-indicator.component.html',
  styleUrl: './attitude-indicator.component.css'
})
export class AttitudeIndicatorComponent {

  @Input() fc_state_overview : FlightControllerStateOverview = FlightControllerStateOverview.Default();

  protected readonly Math = Math; 

  readonly size = 300;
  readonly pitchScale = 5;
  readonly rollStep = 10;
  readonly pitchStep = 5;

  get center(): number {
    return this.size / 2;
  }

  /**
   * Transformation du ciel + sol.
   */
  get horizonTransform(): string {
    const y = -this.fc_state_overview.navigation.pitch_deg * this.pitchScale;

    return `
      translate(${this.center} ${this.center})
      rotate(${-this.fc_state_overview.navigation.roll_deg})
      translate(${-this.center} ${y - this.center})
    `;
  }

  /**
   * Graduations de tangage.
   */
  get pitchMarks(): number[] {
    const count = 8;

    return Array.from(
      { length: count * 2 + 1 },
      (_, i) => (i - count) * this.pitchStep
    );
  }

  /**
   * Graduations de roulis.
   */
  get rollMarks(): number[] {
    const count = 6;

    return Array.from(
      { length: count * 2 + 1 },
      (_, i) => (i - count) * this.rollStep
    );
  }

  /**
   * Taille des graduations de roulis.
   */
  rollMarkLength(angle: number): number {
    return angle % 30 === 0
      ? this.size * 0.055
      : this.size * 0.035;
  }

  /**
   * Taille de la police en fonction de la taille du PFD.
   */
  get rollLabelFontSize(): number {
    return this.size * 0.032;
  }
}