#pragma once

namespace jeayeson
{
  /* To customize jeayeson internals, change these types
   * to any other compatible types which suit your needs.
   * Add any required #includes here.
   */
  template <>
  struct config<config_tag>
  {
    using float_t = double;
    using int_t = int64_t;

    template <typename K, typename V>
    using map_t = std::map<K, V>;
  };
}
