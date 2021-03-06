/* 
 * File:   SOMType.h
 * Author: hans
 *
 * Created on 30 de Março de 2012, 13:15
 */

#ifndef CLUSTERINGSOM_H
#define	CLUSTERINGSOM_H

#include <vector>
#include <map>
#include <iomanip>
#include <iostream>
#include "ArffData.h"
#include "MatVector.h"

#include "SOM.h"
#include "DSNode.h"
#include "ClusteringMetrics.h"
#include "mat.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp" 
#include "opencv2/imgproc/imgproc.hpp"
#include "engine.h"
#include <armadillo>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/* one of the following will define PATH_MAX */
#include <limits.h> 
#include <sys/param.h>


using namespace cv;
using namespace std;

template <class SOMType>
class ClusteringSOM {

public:
    SOMType *som;
    MatMatrix<float> *trainingData;
    bool allocated;
    std::vector<int> groups;
    std::map<int, int> groupLabels; 
    std::vector<int> winn;
    std::string filename;
    bool isSubspaceClustering;
    bool filterNoise;

public:
    ClusteringSOM(){
        isSubspaceClustering = true;
        trainingData = NULL;
        allocated = false;        
        filterNoise = true;
    };

    ClusteringSOM(SOMType *som) {
        this->som = som;
        isSubspaceClustering = true;
        trainingData = NULL;
        allocated = false;        
        filterNoise = true;
    };

    virtual ~ClusteringSOM(){
        if (allocated)
            delete trainingData;
    };

    void setFilterNoise(bool filter) {
        filterNoise = filter;
    }
    
    void setIsSubspaceClustering(bool isSubspaceClustering) {
        this->isSubspaceClustering = isSubspaceClustering;
    }

    virtual int getMeshSize() = 0;

    int getInputSize() {
        return trainingData->cols();
    }
    
    int getNumSamples() {
        return trainingData->rows();
    }
    
    virtual int getNodeId(int node_i) {
        return node_i;
    }

    virtual void train(MatMatrix<float> &trainingData, int N) = 0;
    
    virtual void getRelevances(int node_i, MatVector<float> &relevances) = 0;
    
    virtual void getWeights(int node_i, MatVector<float> &weights) = 0;

    virtual void getWinners(const MatVector<float> &sample, std::vector<int> &winners) = 0;
    
    virtual int getWinner(const MatVector<float> &sample) = 0;
    
    virtual bool isNoise(const MatVector<float> &sample) {
        return false;
    }
        
    void cleanUpTrainingData() {
        trainingData->clear();
        groups.clear();
        groupLabels.clear();
        winn.clear();
    }

    bool readFile(std::string &filename) {
        
        this->filename = filename;
        if (trainingData==NULL && !allocated) {
            trainingData = new MatMatrix<float>();
            allocated = true;
        }
        
        if (ArffData::readArff(filename, *trainingData, groupLabels, groups)) {
                ArffData::rescaleCols01(*trainingData);
                return true;
        }
       /*
        std::vector<double> vectorX;
        std::vector<double> vectorY;
        std::vector<double> vectorS;
        // open MAT-file
        MATFile *pmat = matOpen(filename.c_str(), "r");
        if (pmat == NULL) return false;

        // extract the specified variable
        mxArray *arrx = matGetVariable(pmat, "x");
        mxArray *arry = matGetVariable(pmat, "y");
        mwSize num;
        double *pr;
        if (arrx != NULL && mxIsDouble(arrx) && !mxIsEmpty(arrx)) {
            // copy data
            num = mxGetNumberOfElements(arrx);
            pr = mxGetPr(arrx);
            if (pr != NULL) {
                vectorX.resize(num);
                vectorX.assign(pr, pr+num);
            }
        }
        
        int dimX = mxGetDimensions_700(arrx)[2];
        int dimY = mxGetDimensions_700(arrx)[1];
        int dimZ = mxGetDimensions_700(arrx)[0];
        vector<vector<vector<double> > > arrayXData;    // 3D array definition;
        
        // Set up sizes. (HEIGHT x WIDTH)
        arrayXData.resize(dimX);
        for (int i = 0; i < dimX; ++i) {
          arrayXData[i].resize(dimY);

          for (int j = 0; j < dimY; ++j)
            arrayXData[i][j].resize(dimX);
        }

        int width_index = 0, height_index = 0, depth_index = 0;
        for (int index=0; index<vectorX.size(); ++index){
            width_index=index/(dimY*dimZ);  //Note the integer division . This is x
            height_index=(index-width_index*dimY*dimZ)/dimZ; //This is y
            depth_index=index-width_index*dimY*dimZ- height_index*dimZ;//This is z
            //printf(" %d %d %d %f \n", depth_index, height_index, width_index, v[index]);
            arrayXData[depth_index][height_index][width_index] = vectorX[index];
        }
        
        for(int y = 0; y < mxGetDimensions_700(arrx)[1]; ++y) {
           
            for(int x = 0; x < mxGetDimensions_700(arrx)[0]; ++x) {
                //printf("%f " ,arrayXData[x][y][0]);
            }
            //printf("\n");
        }
        
        if (arry != NULL && mxIsDouble(arry) && !mxIsEmpty(arry)) {
            // copy data
            num = mxGetNumberOfElements(arry);
            pr = mxGetPr(arry);
            if (pr != NULL) {
                vectorY.resize(num);
                vectorY.assign(pr, pr+num);
            }
        }
        
        dimX = mxGetDimensions_700(arry)[2];
        dimY = mxGetDimensions_700(arry)[1];
        dimZ = mxGetDimensions_700(arry)[0];
        vector<vector<vector<double> > > arrayYData;

        // Set up sizes. (HEIGHT x WIDTH)
        arrayYData.resize(dimX);
        for (int i = 0; i < dimX; ++i) {
          arrayYData[i].resize(dimY);

          for (int j = 0; j < dimY; ++j)
            arrayYData[i][j].resize(dimX);
        }
        
        width_index = 0, height_index = 0, depth_index = 0;
        for (int index=0; index<vectorY.size(); ++index){
            width_index=index/(dimY*dimZ);  //Note the integer division . This is x
            height_index=(index-width_index*dimY*dimZ)/dimZ; //This is y
            depth_index=index-width_index*dimY*dimZ- height_index*dimZ;//This is z
            //printf(" %d %d %d %f \n", depth_index, height_index, width_index, v[index]);
            arrayYData[depth_index][height_index][width_index] = vectorY[index];
        }
        
        for(int y = 0; y < mxGetDimensions_700(arry)[1]; ++y) {
           
            for(int x = 0; x < mxGetDimensions_700(arry)[0]; ++x) {
                //printf("%f " ,arrayYData[x][y][0]);
            }
            //printf("\n");
        }
        
        mxArray *s = matGetVariable(pmat, "s");
        if (s != NULL && mxIsDouble(s) && !mxIsEmpty(s)) {
            // copy data
            num = mxGetNumberOfElements(s);
            pr = mxGetPr(s);
            if (pr != NULL) {
                vectorS.resize(num);
                vectorS.assign(pr, pr+num);
            }
        }
        
        for(int x = 0; x < mxGetDimensions_700(s)[0]; ++x) {
                //printf("%f \n" ,vectorS[x]);
            }
        
       arrayXData.erase(arrayXData.begin() + 2);
       //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
       for(int y = 0; y < mxGetDimensions_700(arry)[1]; ++y) {
           
            for(int x = 0; x < mxGetDimensions_700(arry)[0] - 1; ++x) {
              // printf("%f " ,arrayXData[x][y][28]);
            }
            //printf("\n");
        }
       
       arma::cube A(dimZ-1, dimX,dimY);
       
       for(int z = 0; z < dimX; ++z) {
            for(int y = 0; y < dimY; ++y) {
                for(int x = 0; x < dimZ - 1; ++x) {
                    A.at(x,z,y) = arrayXData[x][y][z];                  
                }               
            }
       }

       arma::mat C = reshape( arma::mat(A.memptr(), A.n_elem, 1, false), 2*dimX, dimY);
       //arma::mat D = C.t();
       
       MatMatrix<float> *data = new MatMatrix<float>();
        for(int y = 0; y < dimY; ++y) {
       
            MatVector<float> row;
            for(int x = 0; x < dimX*2; ++x) {
                row.append(C.at(x,y));  
                //printf("%f ", C.at(x,y));                                    
            }
            data->concatRows(row);
            //printf("\n");
            //printf("%d \n", data->size());
        }
        
        //printf("%d ", data->size());
        //printf("%d \n", trainingData->size());
        setData(*data);
        //printf("%d \n", trainingData->size());
                
        for(int x = 0; x < mxGetDimensions_700(s)[0]; ++x) {
            //printf("%f \n" ,vectorS[x]);
            groups.push_back(vectorS[x]);
            bool itemFound = false;
            int itemIndex;
            for (std::map<int, int>::iterator it = groupLabels.begin(); it != groupLabels.end(); it++) {
                if (it->second == vectorS[x]) {
                    itemFound = true;
                    itemIndex = it->first;
                    break;
                }
            }
            if (!itemFound) {
                itemIndex = groupLabels.size();
                groupLabels[itemIndex] = vectorS[x];
            }
            //printf("%d \n", groups[x]);            
        }
       
        for (std::map<int, int>::iterator it = groupLabels.begin(); it != groupLabels.end(); it++) {
            //printf("%d %d \n", it->first, it->second);   
        }
        return true;
        //return false;*/
    }

    void setData(MatMatrix<float> &data) {
        if (allocated) {
            delete trainingData;
            allocated = false;
        }
        
        trainingData = &data;
    }

    void trainSOM(int N) {

        train(*trainingData, N);
    }

    bool writeClusterResults(const std::string &filename) {

        std::ofstream file;
        file.open(filename.c_str());

        if (!file.is_open()) {
            dbgOut(0) << "Error openning output file" << endl;
            return false;
        }       

        int meshSize = getMeshSize();
        int inputSize = getInputSize();

        //file << meshSize << "\t" << inputSize << endl;

        for (int i = 0; i < meshSize; i++) {
            MatVector<float> relevances;
            getRelevances(i, relevances);
            
            //file << i << "\t";
            for (int j = 0; j < inputSize; j++) {
                //file << relevances[j];
                //if (j != inputSize - 1)
                    //file << "\t";
            }
            //file << endl;
        }
        
        std::vector<int> newGroups;
        for (int i = 0; i < trainingData->rows(); i++) {
            MatVector<float> sample;
            
            trainingData->getRow(i, sample);
            //if (filterNoise && isNoise(sample))
            //    continue;
            
            std::vector<int> winners;
            if (isSubspaceClustering) {
                getWinners(sample, winners);
            } else {
                winners.push_back(getWinner(sample));
                winn.push_back(getWinner(sample));
                newGroups.push_back(-9999999);
            }

            for (int j = 0; j < winners.size(); j++) {
                //file << i << "\t";
                file << winners[j] << "\t";
                //file << winners[j];
                //file << endl;
            }
            file << endl;
        }
        
        return true;
    }

    
    std::string outClusters(bool printData = true) {
      
        std::stringstream out;
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        int inputSize = getInputSize();
        std::vector<int> clusterData; 
        for (int i = 0; i < meshSize; i++) {
            
            out << getNodeId(i) << ": ";
            
            MatVector<float> relevances;
            getRelevances(i, relevances);
            
            //Print relevances
            float average = 0;
            int num = 0;
            for (int j = 0; j < inputSize; j++) {
                if (relevances[j]<=1) {
                    average += relevances[j];
                    num++;
                }
            }
            if (num<1) num = 1;
            average = average/num;
            
            for (int j = 0; j < inputSize; j++) {
                if (relevances[j]>average)
                    out << 1 << " ";
                else
                    out << 0 << " ";
            }
            
            clusterData.clear();
            for (int k = 0; k < trainingData->rows(); k++) {
                MatVector<float> sample;
                trainingData->getRow(k, sample);
                if (isNoise(sample))
                    continue;

                std::vector<int> winners;
                if (isSubspaceClustering) {
                        getWinners(sample, winners);
                } else {
                        winners.push_back(getWinner(sample));
                }

                for (int j = 0; j < winners.size(); j++) {
                    if (winners[j] == i) {
                        clusterData.push_back(k);
                        break;
                    }
                }
            }
            
            out << clusterData.size();
            if (printData) {
                out << "\t";
                for (int j = 0; j < clusterData.size(); j++) {
                    out << clusterData[j] << " ";
                }
            }
            out << endl;    
        }

        return out.str();  
    }
    
    std::string outRelevances() {
      
        std::stringstream out;
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        int inputSize = getInputSize();

        out << "D:\t";
        for (int j = 0; j < inputSize; j++) {
                out << j << "\t";
        }
        out << endl; 
        
        for (int i = 0; i < meshSize; i++) {
            
            out << getNodeId(i) << ":\t";
            
            MatVector<float> relevances;
            getRelevances(i, relevances);
            
            //Print relevances
            for (int j = 0; j < inputSize; j++) {
                    out << relevances[j] << "\t";
            }
            out << endl;    
        }

        return out.str();  
    }
    
    std::string outWeights() {
      
        std::stringstream out;
        
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        int inputSize = getInputSize();

        out << "D:\t";
        for (int j = 0; j < inputSize; j++) {
                out << j << "\t";
        }
        out << endl; 
        
        for (int i = 0; i < meshSize; i++) {
            
            out << getNodeId(i) << ":\t";
            
            MatVector<float> weights;
            getWeights(i, weights);
            
            //Print relevances
            for (int j = 0; j < inputSize; j++) {
                    out << weights[j] << "\t";
            }
            out << endl;    
        }

        return out.str();  
    }    
    
    std::string outClassInfo() {
      
        std::stringstream out;
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        int hits = 0;
        int total = 0;
        int noise = 0;
        
        MatVector<int> nodeHits(meshSize);
        MatVector<int> nodeClusterSize(meshSize);
        nodeHits.fill(0);
        nodeClusterSize.fill(0);
        
        for (int k = 0; k < trainingData->rows(); k++) {
            MatVector<float> sample;
            trainingData->getRow(k, sample);
            /*if (isNoise(sample)) {
                noise++;
                continue;
            }*/

            int classIndex = sample.size()-1;
            int winner = getWinner(sample);
            MatVector<float> weights;
            getWeights(winner, weights);

            if (fabs(sample[classIndex] - weights[classIndex]) < 0.5) {
                hits++;
                nodeHits[winner]++;
            }
            
            total++;
            nodeClusterSize[winner]++;
        }
        
        for (int i = 0; i < meshSize; i++) {

            MatVector<float> weights;
            getWeights(i, weights);
            
            out << getNodeId(i) << ": ";
            out << "\t" << weights[weights.size()-1];
            out << "\t" << nodeHits[i] << "/" << nodeClusterSize[i];
            out << "\t" << nodeHits[i]/(float)nodeClusterSize[i];
            out << endl;    
        }

        out << "Classification acuracy:\t" << hits/(float)total << endl;
        out << "Total noise:\t" << noise/(float)trainingData->rows() << endl;
       
        return out.str();
    }
    
    std::string outClassError() {
        
        std::ofstream file;
        file.open((filename + ".ErrorResults").c_str() , std::ios::app);
        
        if (!file.is_open()) {
            dbgOut(0) << "Error openning output file" << endl;
        }
        
        std::stringstream out;
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        int hits = 0;
        int total = 0;
        int noise = 0;
        
        MatVector<int> nodeHits(meshSize);
        MatVector<int> nodeClusterSize(meshSize);
        nodeHits.fill(0);
        nodeClusterSize.fill(0);
        
        for (int k = 0; k < trainingData->rows(); k++) {
            MatVector<float> sample;
            trainingData->getRow(k, sample);
            if (isNoise(sample)) {
                noise++;
                continue;
            }

            int classIndex = sample.size()-1;
            int winner = getWinner(sample);
            MatVector<float> weights;
            getWeights(winner, weights);

            if (fabs(sample[classIndex] - weights[classIndex]) < 0.5) {
                hits++;
                nodeHits[winner]++;
            }
            
            total++;
            nodeClusterSize[winner]++;
        }
        
        for (int i = 0; i < meshSize; i++) {

            MatVector<float> weights;
            getWeights(i, weights);
            
            out << getNodeId(i) << ": ";
            //file << getNodeId(i) << ": ";
            out << "\t" << weights[weights.size()-1];
            //file << "\t" << weights[weights.size()-1];
            out << "\t" << nodeHits[i] << "/" << nodeClusterSize[i];
            //file << "\t" << nodeHits[i] << "/" << nodeClusterSize[i];
            out << "\t" << nodeHits[i]/(float)nodeClusterSize[i];
            //file << "\t" << nodeHits[i]/(float)nodeClusterSize[i];
            out << endl;    
            //file << endl;    
        }

        out << "Classification Error:\t" << 1 - hits/(float)total << endl;
        file << 1 - hits/(float)total << " ,";
        //out << "Classification acuracy:\t" << hits/(float)total << endl;
        out << "Total noise:\t" << noise/(float)trainingData->rows() << endl;
        //file << "Total noise:\t" << noise/(float)trainingData->rows() << endl;
       
        return out.str();
    }
    
    std::string outConfusionMatrix(const std::string filename) {
      
        std::ofstream file;
        file.open(filename.c_str(), std::ios::app);

        if (!file.is_open()) {
            dbgOut(0) << "Error openning output file" << endl;
        }

        std::stringstream out;
        out << std::setprecision(2) << std::fixed;
        
        int meshSize = getMeshSize();
        MatMatrix<int> confusionMatrix(getMeshSize(), groupLabels.size());
        confusionMatrix.fill(0);
        int noise = 0;
        
        for (int k = 0; k < trainingData->rows(); k++) {
            MatVector<float> sample;
            trainingData->getRow(k, sample);
            //if (isNoise(sample) && filterNoise) {
            //    noise++;
            //    continue;
            //}

            std::vector<int> winners;
            if (isSubspaceClustering) {
                getWinners(sample, winners);
            } else {
                winners.push_back(getWinner(sample));
            }
            
            for (int w=0; w<winners.size(); w++) {
                confusionMatrix[winners[w]][groups[k]]++;
            }
        }

        /** print confusion matrix **/
        MatVector<int> rowSums(confusionMatrix.rows());
        MatVector<int> colSums(confusionMatrix.cols());
        rowSums.fill(0);
        colSums.fill(0);        
        dbgOut(0) << "cluster\\class\t|";
        file << "cluster\\class\t|";
        
        for (int c = 0; c < confusionMatrix.cols(); c++){
             dbgOut(1) << "\tcla" << groupLabels[c];
             file << "\tcla" << groupLabels[c];
        }
           
        dbgOut(0) << "\t| Sum" << endl;
        file << "\t| Sum" << endl;
        
        for (int r = 0; r < confusionMatrix.rows(); r++) {
            dbgOut(0) << "clu" << r << "\t\t|";
            file << "clu" << r << "\t\t|";
            for (int c = 0; c < confusionMatrix.cols(); c++) {
                dbgOut(0) << "\t" << confusionMatrix[r][c];
                file << "\t" << confusionMatrix[r][c];
                rowSums[r] += confusionMatrix[r][c];
                colSums[c] += confusionMatrix[r][c];
            }
            dbgOut(0) << "\t| " << rowSums[r] << endl;
            file << "\t| " << rowSums[r] << endl;
        }
        dbgOut(0) << "Sums\t\t|";
        file << "Sums\t\t|";
        for (int c = 0; c < confusionMatrix.cols(); c++){
            dbgOut(0) << "\t" << colSums[c];
            file << "\t" << colSums[c];
        }
            
        dbgOut(0) << "\t| " << colSums.sum() << endl << endl;
        file << "\t| " << colSums.sum() << endl << endl;
        /***/

        dbgOut(0) << "Random index: " << ClusteringMetrics::RANDI(confusionMatrix) << endl;
        file << "Random index: " << ClusteringMetrics::RANDI(confusionMatrix) << endl;
        dbgOut(0) << "Adjusted random index: " << ClusteringMetrics::ARI(confusionMatrix) << endl << endl;
        file << "Adjusted random index: " << ClusteringMetrics::ARI(confusionMatrix) << endl << endl;
        out << "Total noise:\t" << noise << "(" << noise/(float)trainingData->rows() << ")" << endl;
       
        return out.str();
    }
    
    std::string printConditionalEntropy(std::vector<int> &groups) {
        std::stringstream out;
        MatVector<int> trueClusters(groups.size());
        MatVector<int> obtained(groups.size());
        int noise = 0;
        
        for (int k = 0; k < trainingData->rows(); k++) {
            MatVector<float> sample;
            trainingData->getRow(k, sample);
            
//            if (isNoise(sample)) {
//                noise++;
//                continue;
//            }

            int w = getWinner(sample);
            
            trueClusters[k] = groups[k];
            obtained[k] = w;        
            //dbgOut(0) << trueClusters[k] << "\t" << obtained[k] << endl;
            
        }
        
        out << "Conditional entropy:\t" << conditionalEntropy(trueClusters, obtained) << endl;
        out << "Total noise:\t" << noise << "(" << noise/(float)trainingData->rows() << ")" << endl;
        return out.str();
    }
    
    float conditionalEntropy(MatVector<int> &X, MatVector<int> &Y) {
        std::map<int, int> m;

        for (int i=0; i<Y.size(); i++) {
            int cluster = Y[i];

            if (m.find(cluster)!=m.end()) {
                int count = m[cluster];
                m[cluster] = count + 1;
            } else {
                m[cluster] = 1;
            }
        }
        
        for (int i=0; i<X.size(); i++) {
            int cluster = X[i];

            if (m.find(cluster)!=m.end()) {
                int count = m[cluster];
                m[cluster] = count + 1;
            } else {
                m[cluster] = 1;
            }
        }

        int ny = m.size();

        MatVector<double> py(ny);
        MatMatrix<double> pxDy(ny, ny);
        py.fill(0);
        pxDy.fill(0);

        //Count y and (x,y) ocurrences
        for (int i=0; i<Y.size(); i++) {
              pxDy[X[i]][Y[i]]++;
              py[Y[i]]++;
        }

        //Calculate p(x|y)
        for (int x=0; x<ny; x++) {
            for (int y=0; y<ny; y++) {
                if (py[y]>0)
                    pxDy[x][y]/=py[y];
                else 
                    pxDy[x][y] = 0;
            }
        }

        //Calculate p(y)
        for (int y=0; y<ny; y++) {
            py[y]/=Y.size();
        }

        //dbgOut(0) << py.toString() << endl;
        //Compute conditional entropy: H(X|Y) = sum_y{py*sum_x[p(x|y)*log(1/p(x|y))]}
        double hxDy = 0;
        for (int y=0; y<ny; y++) {

            float sum_pxDylog1_pxDy = 0;
            for (int x=0; x<ny; x++) {
                if (pxDy[x][y]>0)
                    sum_pxDylog1_pxDy += pxDy[x][y]*log2(1/pxDy[x][y]);
            }
            hxDy += py[y]*sum_pxDylog1_pxDy;
        }

        return hxDy;
    }
};

#ifdef SOM2D_H
class ClusteringSOM2D: public ClusteringSOM<SOM2D<DSNeuron, SOMParameters> > {

public:
    ClusteringSOM2D(SOM2D<DSNeuron, SOMParameters> *som) : ClusteringSOM<SOM2D<DSNeuron, SOMParameters> >(som){};

    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }

    void getRelevances(int node_i, MatVector<float> &relevances) {
        relevances.size(som->parameters.NFeatures);
        relevances.fill(1);
    }
    
    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.weights;
    }

    int getWinner(const MatVector<float> &sample) {
        
        DSNeuron bmu;
        som->findBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {

        winner.clear();

        DSNeuron bmu;
        som->findBMU(sample, bmu);
        winner.push_back(bmu.c + bmu.r * som->getSomCols());
    }
};
#endif

#ifdef DSLVQ_H
class ClusteringDSLVQ: public ClusteringSOM<DSSOM<LVQNeuron, DSSOMParameters> > {

public:
    ClusteringDSLVQ(DSSOM<LVQNeuron, DSSOMParameters> *som) : ClusteringSOM<DSSOM<LVQNeuron, DSSOMParameters> >(som){};

    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }
    
    void test(MatMatrix<float> &testData, MatVector<int> classes) {
        
        classes.size(testData.rows());
        
        for (int r=0; r<testData.rows(); r++) {
            MatVector<float> sample;
            testData.getRow(r, sample);
            
            LVQNeuron neuron;
            som->findBMU(sample, neuron);
            classes[r] = neuron.getClass();
        }
    }    

    void getRelevances(int node_i, MatVector<float> &relevances) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        LVQNeuron neuron(row, col);
        som->getNeuron(neuron);

        relevances = neuron.dsWeights;
    }

    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        LVQNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.weights;
    }
        
    int getWinner(const MatVector<float> &sample) {
        
        LVQNeuron bmu;
        som->findBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {

        winner.clear();

        LVQNeuron bmu;
        som->findBMU(sample, bmu);
        winner.push_back(bmu.c + bmu.r * som->getSomCols());
    }
};
#endif

#ifdef _DSSOMC_H
class ClusteringDSSOMC: public ClusteringSOM<DSSOMC<DSCNeuron, DSSOMCParameters> > {

public:
    ClusteringDSSOMC(DSSOMC<DSCNeuron, DSSOMCParameters> *som) : ClusteringSOM<DSSOMC<DSCNeuron, DSSOMCParameters> >(som){};

    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }
    
    void getRelevances(int node_i, MatVector<float> &relevances) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSCNeuron neuron(row, col);
        som->getNeuron(neuron);

        relevances = neuron.dsWeights;
    }

    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSCNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.weights;
    }

    int getWinner(const MatVector<float> &sample) {
        
        DSCNeuron bmu;
        som->findBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }

    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {

        winner.clear();

        DSCNeuron bmu;
        som->findBMU(sample, bmu);
        winner.push_back(bmu.c + bmu.r * som->getSomCols());
    }
};
#endif

#ifdef _SOMAW_H
class ClusteringSOMAW: public ClusteringSOM<SOMAW<DSNeuron, SOMAWParameters> > {

public:
    ClusteringSOMAW(SOMAW<DSNeuron, SOMAWParameters> *som) : ClusteringSOM<SOMAW<DSNeuron, SOMAWParameters> >(som){};

    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }

    void getRelevances(int node_i, MatVector<float> &relevances) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        relevances = neuron.dsWeights;
    }
    
    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.weights;
    }

    int getWinner(const MatVector<float> &sample) {
        
        DSNeuron bmu;
        som->findBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {

        winner.clear();

        DSNeuron bmu;
        som->findBMU(sample, bmu);
        winner.push_back(bmu.c + bmu.r * som->getSomCols());
    }
};
#endif

#ifdef _DSSOM_H
class ClusteringDSSOM: public ClusteringSOM<DSSOM<DSNeuron, DSSOMParameters> > {

public:
    ClusteringDSSOM(DSSOM<DSNeuron, DSSOMParameters> *som) : ClusteringSOM<DSSOM<DSNeuron, DSSOMParameters> >(som){};

    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }

    void getRelevances(int node_i, MatVector<float> &relevances) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        relevances = neuron.dsWeights;
    }

    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.weights;
    }
        
    int getWinner(const MatVector<float> &sample) {
        
        DSNeuron bmu;
        som->findBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {

        winner.clear();

        DSNeuron bmu;
        som->findBMU(sample, bmu);
        winner.push_back(bmu.c + bmu.r * som->getSomCols());
    }
};
/*
class ClusteringDSSOM: public ClusteringSOM<DSSOM> {

public:
    ClusteringDSSOM(DSSOM *som) : ClusteringSOM<DSSOM>(som){};
    
    int getMeshSize() {
        return som->getSomCols() * som->getSomRows();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->parameters.NFeatures = trainingData.cols();
        som->parameters.tmax = N;
        som->initializeP(som->parameters);
        som->train(trainingData);
    }
    
    void getRelevances(int node_i, MatVector<float> &relevances) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        relevances = neuron.dsWeights;
    }

    void getWeights(int node_i, MatVector<float> &weights) {
        int col = node_i / som->getSomRows();
        int row = ((node_i / (float) som->getSomRows()) - col) * som->getSomRows();

        DSNeuron neuron(row, col);
        som->getNeuron(neuron);

        weights = neuron.dsWeights;
    }
    
    int getWinner(const MatVector<float> &sample) {
        
        DSNeuron bmu;
        som->findFirstBMU(sample, bmu);
        return bmu.c + bmu.r*som->getSomCols();
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {
    
        DSNeuron bmu;
        som->resetRelevances();
        int k=1;
        while (som->getRelevance().max()>som->parameters.epsilonRho && k<=som->parameters.numWinners)
        {
            if (k==1) {
                float activation = som->findFirstBMU(sample, bmu);
                if (activation < som->parameters.outliersThreshold) {
                    winner.push_back(-1);
                    break;
                }
            }
            else {
                som->findNextBMU(sample, bmu);
            }
            winner.push_back(bmu.c + bmu.r*som->getSomCols());
            som->updateRelevances(bmu);
            k = k + 1;
        }
    }
};
 */
#endif

#ifdef SOM_H_
class ClusteringMeshSOM: public ClusteringSOM<SOM<DSNode> > {

public:
    ClusteringMeshSOM(SOM<DSNode> *som) : ClusteringSOM<SOM<DSNode> >(som){};
    
    int getMeshSize() {
        return som->meshNodeSet.size();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->data = trainingData;
        //som->enumerateNodes();
        //som->trainning(N);
        //std::ofstream file;
        //file.open((filename + ".SumOfSquaredErrors").c_str(), std::ios::app);

        /*if (!file.is_open()) {
            dbgOut(0) << "Error openning output file" << endl;
        }*/
        for (int i=0; i<N; i++){
            som->trainningStep();
            //file << sumOfSquaredError(*som) << " ,";
            //dbgOut(1) << outClassError() << endl;
        }
    }
    
    void getRelevances(int node_i, MatVector<float> &relevances) {
        SOM<DSNode>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if (i==node_i) {
                relevances = (*it)->ds;
                return;
            }
        }
        return;
    }
    
    void getWeights(int node_i, MatVector<float> &weights) {
        SOM<DSNode>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if (i==node_i) {
                weights = (*it)->w;
                return;
            }
        }
        return;
    }    

    int getWinner(const MatVector<float> &sample) {
        DSNode *winner = som->getWinner(sample);
        return getNodeIndex(*winner);
    }
        
    void getWinners(const MatVector<float> &sample, std::vector<int> &winners) {
        DSNode *winner = som->getFirstWinner(sample);
        winners.push_back(getNodeIndex(*winner));
        
        winner = som->getNextWinner(winner);
        while (winner!=NULL) {
            winners.push_back(getNodeIndex(*winner));
            winner = som->getNextWinner(winner);
        }
    }
    
    virtual bool isNoise(const MatVector<float> &sample) {
        return som->isNoise(sample);
    }
    
    int getNodeIndex(DSNode &node) {
        SOM<DSNode>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if ((*it)== &node) {
                return i;
            }
        }
        return -1;
    }
    
    int getNodeId(int node_i) {
        SOM<DSNode>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if (i==node_i) {                
                return (*it)->getId();
            }
        }
        return -1;
    }
};

class ClusteringMeshSOMNodeW: public ClusteringSOM<SOM<NodeW> > {

public:
    ClusteringMeshSOMNodeW(SOM<NodeW> *som) : ClusteringSOM<SOM<NodeW> >(som){};

    int getMeshSize() {
        return som->meshNodeSet.size();
    }

    void train(MatMatrix<float> &trainingData, int N) {
        som->data = trainingData;
        som->trainning(N);
    }

    void getRelevances(int node_i, MatVector<float> &relevances) {
        SOM<NodeW>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        relevances.size((*it)->w.size());
        relevances.fill(1);
        return;
    }

    void getWeights(int node_i, MatVector<float> &weights) {
        SOM<NodeW>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if (i==node_i) {
                weights = (*it)->w;
                return;
            }
        }
        return;
    } 

    int getWinner(const MatVector<float> &sample) {
        NodeW *winner = som->getWinner(sample);
        return getNodeIndex(*winner);
    }

    void getWinners(const MatVector<float> &sample, std::vector<int> &winner) {
        NodeW *winner1 = 0;
        NodeW *winner2 = 0;
        
        som->getWinners(sample, winner1, winner2);
        
        winner.push_back(getNodeIndex(*winner1));
        winner.push_back(getNodeIndex(*winner2));
    }

    int getNodeIndex(Node &node) {
        SOM<NodeW>::TPNodeSet::iterator it = som->meshNodeSet.begin();
        int i=0;
        for (; it != som->meshNodeSet.end(); it++, i++) {
            if ((*it)== &node) {
                return i;
            }
        }
        return -1;
    }
};
#endif

#endif	/* CLUSTERINGSOM_H */
