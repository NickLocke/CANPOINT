bool CanPointMoveToNormal(point point)
{
  // The point switch is set against the movement
  if(point.switch_reverse) return false;

  // A conflicting route has been requested
  if(point.reverse_route_called) return false;

  return true;
}

bool CanPointMoveToReverse(point point)
{
  // The point switch is set against the movement
  if(point.switch_normal) return false;

  // A conflicting route has been requested
  if(point.normal_route_called) return false;

  return true;
  
}