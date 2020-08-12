#!bin/bash

#hadd data_result.root filelist_data_10.root filelist_data_11.root filelist_data_12.root filelist_data_13.root filelist_data_14.root filelist_data_15.root filelist_data_16.root filelist_data_17.root filelist_data_18.root filelist_data_19.root filelist_data_1.root filelist_data_20.root filelist_data_21.root filelist_data_22.root filelist_data_23.root filelist_data_24.root filelist_data_25.root filelist_data_26.root filelist_data_27.root filelist_data_28.root filelist_data_29.root filelist_data_2.root filelist_data_30.root filelist_data_31.root filelist_data_32.root filelist_data_33.root filelist_data_34.root filelist_data_35.root filelist_data_36.root filelist_data_37.root filelist_data_38.root filelist_data_39.root filelist_data_3.root filelist_data_40.root filelist_data_41.root filelist_data_42.root filelist_data_43.root filelist_data_44.root filelist_data_45.root filelist_data_46.root filelist_data_47.root filelist_data_48.root filelist_data_49.root filelist_data_4.root filelist_data_50.root filelist_data_51.root filelist_data_52.root filelist_data_53.root filelist_data_54.root filelist_data_55.root filelist_data_56.root filelist_data_57.root filelist_data_58.root filelist_data_59.root filelist_data_5.root filelist_data_60.root filelist_data_61.root filelist_data_62.root filelist_data_63.root filelist_data_64.root filelist_data_65.root filelist_data_66.root filelist_data_67.root filelist_data_68.root filelist_data_69.root filelist_data_6.root filelist_data_70.root filelist_data_7.root filelist_data_8.root filelist_data_9.root

rm data_result.root

hadd test_1.root filelist_data_10.root filelist_data_11.root filelist_data_12.root filelist_data_13.root filelist_data_14.root filelist_data_15.root
hadd test_2.root filelist_data_16.root filelist_data_17.root filelist_data_18.root filelist_data_19.root filelist_data_1.root filelist_data_20.root
hadd test_3.root filelist_data_21.root filelist_data_22.root filelist_data_23.root filelist_data_24.root filelist_data_25.root filelist_data_26.root
hadd test_4.root filelist_data_27.root filelist_data_28.root filelist_data_29.root filelist_data_2.root filelist_data_30.root filelist_data_31.root
hadd test_5.root filelist_data_32.root filelist_data_33.root filelist_data_34.root filelist_data_35.root filelist_data_36.root filelist_data_37.root
hadd test_6.root filelist_data_38.root filelist_data_39.root filelist_data_3.root filelist_data_40.root filelist_data_41.root filelist_data_42.root
hadd test_7.root filelist_data_43.root filelist_data_44.root filelist_data_45.root filelist_data_46.root filelist_data_47.root filelist_data_48.root
hadd test_8.root filelist_data_49.root filelist_data_4.root filelist_data_50.root filelist_data_51.root filelist_data_52.root filelist_data_53.root
hadd test_9.root filelist_data_54.root filelist_data_55.root filelist_data_56.root filelist_data_57.root filelist_data_58.root filelist_data_59.root
hadd test_10.root filelist_data_5.root filelist_data_60.root filelist_data_61.root filelist_data_62.root filelist_data_63.root filelist_data_64.root
hadd test_11.root filelist_data_65.root filelist_data_66.root filelist_data_67.root filelist_data_68.root filelist_data_69.root filelist_data_6.root
hadd test_12.root filelist_data_70.root filelist_data_7.root filelist_data_8.root filelist_data_9.root

hadd combine_1.root test_1.root test_2.root test_3.root test_4.root test_5.root test_6.root
hadd combine_2.root test_7.root test_8.root test_9.root test_10.root test_11.root test_12.root

hadd data_result.root combine_1.root combine_2.root

rm test_*.root
rm combine_*.root
