# Biovo FP1020A UART library for ESP32

Biovo FP1020A fingerprint sensor module library, for ESP32

## Howto

The User ID mentioned below is a **12 bit positive integer, NOT 16 bit.** i.e. the range is 0x1 to 0xfff.

### Add your fingerprint

1. Run `fp1020a_add_fp_start(user_id, lvl)` at the beginning
2. Then run `fp1020a_add_fp_intermediate(user_id, lvl)` for four times 
3. Finally, run `fp1020a_add_fp_end(user_id, lvl)`

According to the datasheet, the enrollment for one finger need to be repeated for 2 to 6 times.

That means, you have to run Step 1 and Step 3 (for two repeats). Meanwhile, 6 times is recommended for some smaller sensors like FP1020A.

### Verify a fingerprint

If you know an user ID that previously enrolled, then use `fp1020a_auth_user_id(user_id)`.
Otherwise, use `fp1020a_auth_user()`

### Delete a fingerprint

Run either `fp1020a_remove_one_user(user_id)` for removing one record, or `fp1020a_remove_all_user()` to nuke all records.

## License
Anti-996 license for the users (individual and/or organisations) in the whole Greater China (Mainland, Hong Kong, Taiwan & Macau), Japan and Korea.

For more info, please refer to: https://github.com/996icu/996.ICU/blob/master/LICENSE

MIT license for the rest of the world.