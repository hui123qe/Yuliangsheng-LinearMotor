#pragma once

#include "AcquisitionTypes.h"

#include <QString>
#include <QStringList>

class ExcelSessionWriter final
{
public:
    static bool save(const AcquisitionSessionData& session,
                     QStringList* savedFiles,
                     QString* errorMessage);
};
