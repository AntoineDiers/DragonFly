#pragma once

enum class PeerId
{
    FLIGHT_CONTROLLER = 0,
    PAYLOAD_COMPUTER,
    REMOTE_CONTROL,
    GROUND_COMPUTER
};

enum class PeerRelationship
{
    SELF,
    LOCAL,
    DIRECT_REMOTE,
    INDIRECT_REMOTE
};

enum class LinkId
{
    REMOTE,
    LOCAL
};

PeerRelationship getPeerRelationship(PeerId self, PeerId other)
{
    switch (self)
    {
        case PeerId::FLIGHT_CONTROLLER :    
        {  
            switch (other)
            {
                case PeerId::FLIGHT_CONTROLLER : { return PeerRelationship::SELF; }
                case PeerId::PAYLOAD_COMPUTER : { return PeerRelationship::LOCAL; }
                case PeerId::REMOTE_CONTROL :   { return PeerRelationship::DIRECT_REMOTE; }
                case PeerId::GROUND_COMPUTER :  { return PeerRelationship::INDIRECT_REMOTE; }
            }
            break;
        }
        case PeerId::PAYLOAD_COMPUTER :    
        {  
            switch (other)
            {
                case PeerId::PAYLOAD_COMPUTER : { return PeerRelationship::SELF; }
                case PeerId::FLIGHT_CONTROLLER : { return PeerRelationship::LOCAL; }
                case PeerId::GROUND_COMPUTER :   { return PeerRelationship::DIRECT_REMOTE; }
                case PeerId::REMOTE_CONTROL :  { return PeerRelationship::INDIRECT_REMOTE; }
            }
            break;
        }
        case PeerId::REMOTE_CONTROL :    
        {  
            switch (other)
            {
                case PeerId::REMOTE_CONTROL : { return PeerRelationship::SELF; }
                case PeerId::GROUND_COMPUTER : { return PeerRelationship::LOCAL; }
                case PeerId::FLIGHT_CONTROLLER :   { return PeerRelationship::DIRECT_REMOTE; }
                case PeerId::PAYLOAD_COMPUTER :  { return PeerRelationship::INDIRECT_REMOTE; }
            }
            break;
        }
        case PeerId::GROUND_COMPUTER :    
        {  
            switch (other)
            {
                case PeerId::GROUND_COMPUTER : { return PeerRelationship::SELF; }
                case PeerId::REMOTE_CONTROL : { return PeerRelationship::LOCAL; }
                case PeerId::PAYLOAD_COMPUTER :   { return PeerRelationship::DIRECT_REMOTE; }
                case PeerId::FLIGHT_CONTROLLER :  { return PeerRelationship::INDIRECT_REMOTE; }
            }
            break;
        }
    }

    // Should never happen
    return PeerRelationship::SELF;
}