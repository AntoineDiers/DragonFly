import { DiagLevel } from './DiagLevel'

export { DiagLevel }

export interface LinkStats
{
    connection_status : DiagLevel;
    ping_ms : number;
    packet_loss_percent : number;
}

export namespace LinkStats {
    export function Default() : LinkStats {
        return {
            connection_status :     DiagLevel.OK,
            ping_ms :               40,
            packet_loss_percent :   0,
        };
    };
}