
cd $HOME/pictures_ucn56EOF

stop.ws
stop.ws

#rm dbimage.bin

#
# Get options from WSPCS.cfg
#
FDDIOPT=`awk 'match($1,"-FDDI") == 1 {print tolower($2)}' WSPCS.cfg`
# MODEMOPT=`awk 'match($1,"-MODEM") == 1 {print tolower($2)}' WSPCS.cfg`

#
# Build Tdoc file list
#
echo "Building List of Tdoc Files"
echo
echo "Adding the CFM Matrix Page to the file list"
ls -1 PicUCN_CFMmatrix.Tdoc > $HOME/lst/FileList
echo "Adding the CFM 100 Series pages to the file list";
ls -1 Pic1*.Tdoc >> $HOME/lst/FileList
echo "Adding the CFM 200 Series pages to the file list"
ls -1 Pic2*.Tdoc >> $HOME/lst/FileList
echo "Adding the CFM 300 Series pages to the file list"
ls -1 Pic3*.Tdoc >> $HOME/lst/FileList
echo "Adding the Display Header Page to the file list"
ls -1 PicUCN_header.Tdoc >> $HOME/lst/FileList
echo "Adding the Display Footer Page to the file list"
ls -1 PicUCN_footer.Tdoc >> $HOME/lst/FileList
echo "Adding the CEA Alarm Limit Display Page to the file list"
ls -1 PicUCN_CEAAlmLimitDisp.Tdoc >> $HOME/lst/FileList
echo "Adding the COLSS Display Pages to the file list"
ls -1 PicUCN_COLSS*.Tdoc >> $HOME/lst/FileList
echo "Adding the Mid-Loop Display Page to the file list"
ls -1 PicUCN_MIDLOOP.Tdoc >> $HOME/lst/FileList
echo "Adding the PT-Plot Normal Page to the file list"
ls -1 PicUCN_PtPlotNormal.Tdoc >> $HOME/lst/FileList
echo "Adding the PT-Plot Emergenct Page to the file list"
ls -1 PicUCN_PtPlotEmergency.Tdoc >> $HOME/lst/FileList
echo "Adding the UTDV Pages that Display Point-IDs to the file list"
ls -1 PicUCN_UTDVEnterEFPH1.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_UTDVEnterEFPH2.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_UTDVEnterEFPH3.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_UTDVConfirmEFPH2.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_UTDVConfirmEFPH3.Tdoc >> $HOME/lst/FileList
ls -1 Pic508_HFTPic.Tdoc >> $HOME/lst/FileList
echo "Adding the CEA Surveillance Test Page to the file list"
ls -1 PicUCN_CEAPdilTest.Tdoc >> $HOME/lst/FileList
#
# CORENET 2017, by Philkyu KWON.
#
echo "Adding the EQUIPMENT RUNTIME CALCULATION Display Page to the file list"
ls -1 PicUCN_EQUIP_RUNTIME_MENU.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_EQUIP_RUN_STATUS.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_EQUIP_CAL_VALUE.Tdoc >> $HOME/lst/FileList
echo "Adding the PRINT PID FOR TEST Display Page to the file list"
ls -1 PicUCN_PRINT_PID_TEST.Tdoc >> $HOME/lst/FileList
echo "Adding the RCS MASS CALCULATION Display Page to the file list"
ls -1 PicUCN_RCS_MASS_FLOW.Tdoc >> $HOME/lst/FileList
ls -1 PicUCN_RCSLeak_OnLine.Tdoc >> $HOME/lst/FileList

echo

#
# For each Tdoc in the file list build a shopping list only if necessary
#
for file in `cat $HOME/lst/FileList`
do
if [ $HOME/pictures_ucn56EOF/$file -nt $HOME/lst/$file.ShoppingList ]
then
   echo "Building PID list for" $file
   grep "&" $file | grep -Ev -e"&=" -e"&&" -e\" | awk -F\& '{print $2}' | awk -F\` '{print $1}' | awk '$1 == toupper($1) {print $1}' | sort -u > $HOME/lst/$file.ShoppingList
else
   echo "Skipping PID list for" $file
fi
done

#
# Perform initializations for FDDI
#
if [ "$FDDIOPT" = "on" ]
then
   if [ -a fddi.in ]
   then
      rm -f fddi.in
   fi
   echo >fddi.in
   if [ -a fddi.out ]
   then
      mv -f fddi.out fddi.out.old
   fi
fi

#
# Perform initializations for modem
#
# if [ "$MODEMOPT" != "" ] && [ "$MODEMOPT" != "off" ]
# then
#    ppl -o $MODEMOPT
# fi

#
# Start WPSCS task
#
xhost +`hostname`
WSPCS 2>&1 | sysLog -l WSPCS.log

echo "Return from WSPCS in start.ws"

stop.ws

echo "EXITING start.ws"
