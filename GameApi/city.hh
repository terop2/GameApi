
struct CityBlock
{
  // area
  int x_width, z_width;
  int y_height;

  // building
  int building_l,building_t,building_r,building_b; // divide to building vs grass

  int number_of_floors;
  int number_of_apartment_per_floor;
  bool first_floor_shopping;
  bool underground_garage;
  int roof_shape;
  int window_shape;
  int num_garage_doors;
  int num_of_cars;
  bool street_l;
  bool street_r;
  bool street_t;
  bool street_b;
  int num_of_parking_places;
  bool is_shopping_mall;
  bool is_airport;
  bool is_railwaystation;
  bool is_heat_exchange;
  bool is_hotel;
  bool is_train_tracks;
  bool is_factory;
  int factory_product_id;
  
  bool has_electricity;
  bool has_internet;
  bool has_heating;
  bool has_working_landline_phone;
  bool has_mobile_phones;
  bool has_wind_farm;
  bool has_multilayer_windows;
  bool has_pipe;
  bool has_blumbing;
  bool has_water_supply;
  
  bool has_wildlife;
  bool has_insects;
  bool has_bugs;
  bool has_too_hot;
  bool has_too_cold;
  bool has_no_food_supply;
  bool has_no_water;
  bool has_no_electricity;
  bool has_no_internet;
  bool has_no_heating;
  bool has_no_access_to_govt;
  bool has_no_sellable_products;
  int num_sellable_products;
  bool has_access_to_marketing;
  int size_of_available_markets;
  int money_available;
  int money_debt;
  
  
  // life
  int timestep;
  int timestep_speed;
  int family_size;
  int externals_size;
  int externals_num_timesteps;
  int number_of_cars;
  int number_of_tractors;
};
