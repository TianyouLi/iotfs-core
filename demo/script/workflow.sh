#! /bin/sh

cd $1

result=`find . -name light | wc -l`
if [ $result = "0" ]
	then echo No Light Resource Found!!!!; 
	else
		cd $(find . -name light)
		cd $(find . -name core.light)

		result=`find -iname 'name' -exec grep 'John' {} \; | wc -l`
		if [ $result = "0" ]
			then echo No John\'s Light Found!!!!
			else 
				value=`cat power`
				if [ $value = "0" ]
				    then echo true > 'state'
				    else echo false > 'state' 
				fi
		fi
fi


