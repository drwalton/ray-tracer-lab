#pragma once

typedef unsigned int IntersectMask;

const IntersectMask VISIBLE_BITMASK = 0b1;
const IntersectMask SHADOW_BITMASK = 0b10;

const IntersectMask ALL_BITMASK = ~0b0;

const IntersectMask DEFAULT_BITMASK = VISIBLE_BITMASK | SHADOW_BITMASK;


