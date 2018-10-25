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
		m_dataPoints.clear();
	}	

	double GetLargestDataPoint()
	{
		double largestValue = 0.0;
		for (int dataIndex = 0; dataIndex < (int)m_dataPoints.size(); ++dataIndex)
		{
			if(m_dataPoints[dataIndex] > largestValue)
				largestValue = m_dataPoints[dataIndex];
		}

		return largestValue;
	}

	double GetAverageForDataPointCount(int count)
	{
		double sum = 0.0;
		for (int dataIndex = 0; dataIndex < count; ++dataIndex)
		{
			if (dataIndex < (int)m_dataPoints.size())
			{
				sum += m_dataPoints[dataIndex];
			}			
		}
		return sum/count;
	}

	template <class T>
	void UpdateDataSet(const std::vector<T*>& objects, const std::vector<double>& dataPoints)
	{
		m_dataObjects = objects;
		m_dataPoints = dataPoints;
	}

	template <class T>
	void AddDataObject(T* object, const double& dataPoint)
	{
		if (object != nullptr)
		{
			m_dataObjects.push_back(object);
			m_dataPoints.push_back(dataPoint);
		}	
	}

	void ClearContent()
	{
		for (int objectIndex = 0; objectIndex < (int)m_dataObjects.size(); ++objectIndex)
		{
			m_dataObjects[objectIndex] = nullptr;
		}

		m_dataObjects.clear();
		m_dataPoints.clear();
	}
	
	//void GetMeanDataPoint();
	//void GetMinimumDataPoint();

public:
	std::vector<T*> m_dataObjects;
	std::vector<double> m_dataPoints; //generalized so easy to cast to and from
};

