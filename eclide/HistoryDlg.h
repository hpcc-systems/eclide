#pragma once

__interface IAttributeHistory;
__interface IAttribute;

void ShowHistory(IAttribute * attr);
void ShowHistory(IAttributeHistory * current, IAttribute * attr);
void ShowCompare(IAttributeHistory * current, IAttribute * attr);


