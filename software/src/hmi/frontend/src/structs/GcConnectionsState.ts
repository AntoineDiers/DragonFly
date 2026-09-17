import { LinkStats, DiagLevel } from './LinkStats'

export { LinkStats, DiagLevel }

export interface GcConnectionState
{
    fc_via_4g : LinkStats;
    fc_via_rf : LinkStats;
    pc_via_4g : LinkStats;
    pc_via_rf : LinkStats;
    rc : LinkStats;
}

export namespace GcConnectionState {
    export function Default() : GcConnectionState {
        return{
            fc_via_4g :     LinkStats.Default(),
            fc_via_rf :     LinkStats.Default(),
            pc_via_4g :     LinkStats.Default(),
            pc_via_rf :     LinkStats.Default(),
            rc :            LinkStats.Default(),
        };
        
    };
}