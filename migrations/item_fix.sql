UPDATE `ul_item`.`item` SET item_hdr = item_hdr | 2 WHERE (((`ul_item`.`item`.`item_state1` >> 8) & 255) = 11);
