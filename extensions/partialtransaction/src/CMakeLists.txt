cmake_minimum_required(VERSION 3.2)

catapult_define_extension_src(partialtransaction api chain handlers)
target_link_libraries(catapult.partialtransaction catapult.plugins.aggregate.sdk)
