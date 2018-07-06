#pragma once
#include "Engine\Core\Widget.hpp"
#include <vector>

template <class T>
class Graph : public Widget
{
public:
	Graph()
	{

	}

	virtual ~Graph() override
	{
		for (int objectIndex = 0; objectIndex < (int)m_dataObjects.size(); ++objectIndex)
		{
			m_dataObjects[objectIndex] = nullptr;
		}
		m_dataObjects.clear();
	}
	
	/*template <class T>
	RefreshGraph<T>()
	{

	}

	template <class T>
	double GetLargestDataPoint()
	{
		double largestValue = 0.0;
		for (int dataIndex = 0; dataIndex < (int)m_dataPoints.size(); ++dataIndex)
		{
			if(m_dataPoints[dataIndex] > largestValue)
				return largestValue;
		}
	}*/

	template <class T>
	void UpdateDataSet(const std::vector<T*> objects, const std::vector<double>& dataPoints)
	{
		m_dataObjects = objects;
		m_dataPoints = dataPoints;
	}

	template <class T>
	void AddDataObject(T* object, const double& dataPoint)
	{
		m_dataObjects.push_back(object);
		m_dataPoints.push_back(dataPoints);
	}
	
	//void GetMeanDataPoint();
	//void GetMinimumDataPoint();

public:
	std::vector<T*> m_dataObjects;
	std::vector<double> m_dataPoints; //generalized so easy to cast to and from
};

