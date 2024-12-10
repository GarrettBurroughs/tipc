#include "messages.h"


InitializeResponse newInitializeResponse(int id){ 
    return InitializeResponse {
        "2.0",
        id,
        InitializeResult {
            ServerCapabilities {
                1, // Get full document
                true,
                false,
                false
            },
            ServerInfo {
                "tiplsp",
                "0.0.1"
            }
        }
    };
};

HoverResponse newHoverResponse(int id, std::string contents){ 
    return HoverResponse {
        "2.0",
        id,
        HoverResult {
            contents
        }
    };
};
