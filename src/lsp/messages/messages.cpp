#include "messages.h"


InitializeResponse newInitializeResponse(int id){ 
    return InitializeResponse {
        "2.0",
        id,
        InitializeResult {
            ServerCapabilities {
                1 // Get full document
            },
            ServerInfo {
                "tiplsp",
                "0.0.1"
            }
        }
    };
};
