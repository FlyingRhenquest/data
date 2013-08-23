/**
 * Data structure for extra data from flysight
 */

#ifndef _HPP_FLYSIGHT_EXTRA_DATA
#define _HPP_FLYSIGHT_EXTRA_DATA

namespace fr {

  namespace data {

    struct flysight_extra_data {
      double vel_north;
      double vel_east;
      double vel_down;
      double horiz_acc;
      double vert_acc;
      double speed_acc;
      int gps_fix_type;
      int satellites_locked;      
    };

  }
}

#endif
