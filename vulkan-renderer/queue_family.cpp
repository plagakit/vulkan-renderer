#include "queue_family.h"

bool QueueFamilyIndices::IsComplete()
{
    return m_graphicsFamily.has_value();
}
