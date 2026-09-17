import { AfterViewInit, ChangeDetectorRef, Component, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import * as Leaflet from 'leaflet';
import { WebRtcClient } from '../lib/webrtc/webrtc_client';
import { CommonModule } from '@angular/common';
import { AttitudeIndicatorComponent} from '../lib/attitude-indicator/attitude-indicator.component'
import { AltitudeIndicatorComponent } from '../lib/altitude-indicator/altitude-indicator.component'
import { DashboardComponent } from '../lib/dashboard/dashboard.component'
import { StateOverlayComponent } from '../lib/state-overlay/state-overlay.component'
import { ConnectionOverlayComponent } from '../lib/connection-overlay/connection-overlay.component'
import { HttpClient } from '@angular/common/http';
import { FlightControllerStateOverview } from "../structs/FlightControllerStateOverview"
import { GcConnectionState } from '../structs/GcConnectionsState';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, CommonModule, AttitudeIndicatorComponent, AltitudeIndicatorComponent, DashboardComponent, StateOverlayComponent, ConnectionOverlayComponent],
  templateUrl: './app.html',
  styleUrl: './app.css'
})
export class App implements AfterViewInit {

    ping_ms : number | null = null;

    map : Leaflet.Map | null = null;
    marker : Leaflet.Marker | null = null;

    fc_state_overview : FlightControllerStateOverview = FlightControllerStateOverview.Default();
    gc_connection_state : GcConnectionState = GcConnectionState.Default();

    // Video
    _webrtc_client_1 : WebRtcClient | null = null;
    _webrtc_client_2 : WebRtcClient | null = null;

    private updateTimer?: ReturnType<typeof setInterval>;


    constructor(private changeDetector: ChangeDetectorRef, private http: HttpClient) 
    {
        setInterval(()=>
        {
            this.http.get<FlightControllerStateOverview>('http://localhost:1234/flight_controller_state_overview').subscribe({
                next: data => {
                    this.fc_state_overview = data;
                },
                error: err => {
                    console.error('Erreur HTTP', err);
                }
            });

            this.http.get<GcConnectionState>('http://localhost:1234/ground_computer_connection_state').subscribe({
                next: data => {
                    this.gc_connection_state = data;
                },
                error: err => {
                    console.error('Erreur HTTP', err);
                }
            });

            this.changeDetector.detectChanges();
        }, 100);
    }

    updateRandom(value: number, min: number, max: number, variation: number): number {
        value += (Math.random() * 2 - 1) * variation;
        return Math.max(min, Math.min(max, value));
    }

    ngAfterViewInit() 
    {
        this.map = Leaflet.map('map', 
        {
            layers: 
            [
                new Leaflet.TileLayer(
                    'https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', 
                    {attribution: '&copy; OpenStreetMap contributors'} as Leaflet.TileLayerOptions)
            ],
            zoom: 12,
            center: new Leaflet.LatLng(49.44, 1.09),
            zoomControl: false,
        });

        this.marker = Leaflet.marker([0,0], 
        {
            icon: Leaflet.divIcon({
                className: 'leaflet-marker',
                iconSize: [50, 50],
                iconAnchor: [25, 25],
                html: `<img id="leaflet-marker-image" src="/assets/icons/attitude/top.png" width="50" height="50">`
            })
        }).addTo(this.map);

        /*const video_1 = <HTMLVideoElement>document.getElementById("video1")!;
        this._webrtc_client_1 = new WebRtcClient(video_1, `ws://127.0.0.1:8443`);

        const video_2 = <HTMLVideoElement>document.getElementById("video2")!;
        this._webrtc_client_2 = new WebRtcClient(video_2, `ws://127.0.0.1:8444`);

        setInterval(()=>
        { 
            if(!this._webrtc_client_1!.isStreamAlive())
            {
                this._webrtc_client_1 = new WebRtcClient(video_1, `ws://127.0.0.1:8443`);
            }

            if(!this._webrtc_client_2!.isStreamAlive())
            {
                this._webrtc_client_2 = new WebRtcClient(video_2, `ws://127.0.0.1:8444`);
            }
        }, 3000);*/
    }

    updateDragonFlyMarker()
    {
        if(!this.map || !this.marker) { return; }

        /*this.marker.setLatLng([this.latitude_deg, this.longitude_deg]);
        const img = document.getElementById('leaflet-marker-image') as HTMLImageElement;
        if(img)
        {
            img.style.transform = `rotate(${this.heading_deg}deg)`;
        }*/
    }
}
