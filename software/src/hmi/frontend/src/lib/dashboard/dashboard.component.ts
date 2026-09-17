import { Component, Input } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FlightControllerStateOverview } from '../../structs/FlightControllerStateOverview';

@Component({
  selector: 'dashboard',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './dashboard.component.html',
  styleUrl: './dashboard.component.css'
})
export class DashboardComponent {

  @Input() fc_state_overview : FlightControllerStateOverview = FlightControllerStateOverview.Default();

  protected readonly Math = Math;

  readonly center = 150;
  readonly radius = 110;

  // Arc de 270°
  readonly startAngle = 135;
  readonly endAngle = 405;

  readonly tickPercents = Array.from(
    { length: 11 },
    (_, i) => i * 10
  );

  readonly labelPercents = Array.from(
    { length: 6 },
    (_, i) => i * 20
  );


  clampServo(angle: number): number {
    return Math.max(-45, Math.min(45, angle));
    }

    servoPosition(angle: number): number {
    // 0° = centre de la barre
    // -45° = gauche
    // +45° = droite
    return (this.clampServo(angle) + 45) / 90 * 100;
    }

  get clampedThrottle(): number {
    return Math.max(0, Math.min(100, this.fc_state_overview.actuators.throttle_percent));
  }


  get clampedBattery(): number {
    return Math.max(0, Math.min(100, this.fc_state_overview.battery_level_percent));
  }


  angleForPercent(percent: number): number {
    return this.startAngle +
      (this.endAngle - this.startAngle) * percent / 100;
  }


  pointOnCircle(
    angle: number,
    radius: number = this.radius
  ): { x: number, y: number } {

    const rad = angle * Math.PI / 180;

    return {
      x: this.center + radius * Math.cos(rad),
      y: this.center + radius * Math.sin(rad)
    };
  }


  tickStart(percent: number) {
    return this.pointOnCircle(
      this.angleForPercent(percent),
      this.radius + 2
    );
  }


  tickEnd(percent: number) {
    return this.pointOnCircle(
      this.angleForPercent(percent),
      this.radius - 10
    );
  }


  labelPosition(percent: number) {
    return this.pointOnCircle(
      this.angleForPercent(percent),
      this.radius - 25
    );
  }


  isMajorTick(percent: number): boolean {
    return percent % 20 === 0;
  }
}