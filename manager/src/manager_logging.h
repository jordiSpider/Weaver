#pragma once

#include <QString>

// Return the directory used for Manager application logs.
QString managerLogDirectory();
// Create/open the log file and route Qt messages to it.
void initializeManagerLogging();
